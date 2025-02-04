#!/usr/bin/env python3
import json
import math
import requests

from collections import deque

import openpilot.system.sentry as sentry

from cereal import messaging

from openpilot.selfdrive.frogpilot.frogpilot_utilities import calculate_distance_to_point, is_url_pingable
from openpilot.selfdrive.frogpilot.frogpilot_variables import params, params_memory

MAX_ENTRIES = 10_000_000
OVERPASS_API_URL = "http://overpass-api.de/api/interpreter"

class MapSpeedLogger:
  def __init__(self):
    self.speed_limits_checked = False
    self.started_previously = False

    self.previous_coords = None

    self.dataset = deque(json.loads(params.get("SpeedLimits") or "[]"), maxlen=MAX_ENTRIES)
    self.filtered_dataset = deque(json.loads(params.get("SpeedLimitsFiltered") or "[]"), maxlen=MAX_ENTRIES)

    self.sm = messaging.SubMaster(["deviceState", "frogpilotCarState", "frogpilotNavigation"])

  def add_entry(self, dataset, entry):
    dataset.append(entry)

  def log_speed_limit(self):
    self.sm.update()

    if not self.sm["deviceState"].started and self.started_previously:
      self.speed_limits_checked = False
      params.put("SpeedLimits", json.dumps(list(self.dataset)))

    self.started_previously = self.sm["deviceState"].started

    if not self.sm.updated["frogpilotNavigation"]:
      return

    position = json.loads(params_memory.get("LastGPSPosition") or "{}")
    if "latitude" not in position or "longitude" not in position or params_memory.get_float("MapSpeedLimit") != 0:
      self.previous_coords = None
      return

    current_latitude, current_longitude = position["latitude"], position["longitude"]

    if self.previous_coords:
      start_latitude, start_longitude = map(math.radians, [self.previous_coords["latitude"], self.previous_coords["longitude"]])
      end_latitude, end_longitude = map(math.radians, [current_latitude, current_longitude])
      distance = calculate_distance_to_point(start_latitude, start_longitude, end_latitude, end_longitude)

      if distance < 1:
        return
    else:
      self.previous_coords = {"latitude": current_latitude, "longitude": current_longitude}
      return

    dashboard_speed = self.sm["frogpilotCarState"].dashboardSpeedLimit
    navigation_speed = self.sm["frogpilotNavigation"].navigationSpeedLimit

    if dashboard_speed:
      self.add_entry(self.dataset, {
        "start_coordinates": self.previous_coords,
        "end_coordinates": {"latitude": current_latitude, "longitude": current_longitude},
        "speed_limit": dashboard_speed,
        "source": "Dashboard"
      })

    elif navigation_speed:
      self.add_entry(self.dataset, {
        "start_coordinates": self.previous_coords,
        "end_coordinates": {"latitude": current_latitude, "longitude": current_longitude},
        "speed_limit": navigation_speed,
        "source": "NOO"
      })

    self.previous_coords = {"latitude": current_latitude, "longitude": current_longitude}

  def fetch_segments_from_overpass(self, start_coords, end_coords):
    min_lat = min(start_coords.get("latitude"), end_coords.get("latitude")) - 0.0001
    max_lat = max(start_coords.get("latitude"), end_coords.get("latitude")) + 0.0001
    min_lon = min(start_coords.get("longitude"), end_coords.get("longitude")) - 0.0001
    max_lon = max(start_coords.get("longitude"), end_coords.get("longitude")) + 0.0001

    query = (
      f"[out:json]; "
      f"way({min_lat},{min_lon},{max_lat},{max_lon})[highway]; "
      f"out body; >; out skel qt;"
    )

    try:
      response = requests.get(OVERPASS_API_URL, params={"data": query}, timeout=5)
      response.raise_for_status()

      data = response.json()
      ways = [element for element in data.get("elements", []) if element.get("type") == "way"]

      if not ways:
        return None

      segment = ways[0]
      segment_id = segment.get("id")
      maxspeed = segment.get("tags", {}).get("maxspeed")

      try:
        speed_limit = int(maxspeed.split()[0]) if maxspeed else None
      except (ValueError, AttributeError):
        speed_limit = None

      return (segment_id, speed_limit)
    except Exception as e:
      print(f"Unexpected error: {e}")
      return None

  def fetch_speed_limit_for_segment_id(self, segment_id):
    query = f"[out:json]; way({segment_id}); out body;"

    try:
      response = requests.get(OVERPASS_API_URL, params={"data": query}, timeout=5)
      response.raise_for_status()

      data = response.json()
      ways = [element for element in data.get("elements", []) if element.get("type") == "way"]

      if not ways:
        return None

      maxspeed = ways[0].get("tags", {}).get("maxspeed")
      try:
        speed_limit = int(maxspeed.split()[0]) if maxspeed else None
      except (ValueError, AttributeError):
        speed_limit = None
      return speed_limit
    except Exception as e:
      print(f"Unexpected error while fetching speed limit for segment {segment_id}: {e}")
      return None

  def update_speed_limits(self):
    if not self.dataset:
      return

    filtered_cleaned = deque(maxlen=MAX_ENTRIES)
    for entry in self.filtered_dataset:
      self.sm.update()
      if self.sm["deviceState"].started:
        break

      segment_id = entry.get("segment_id")
      if segment_id:
        overpass_speed = self.fetch_speed_limit_for_segment_id(segment_id)
        if overpass_speed is not None:
          continue
        filtered_cleaned.append(entry)

    self.filtered_dataset = filtered_cleaned

    existing_segment_ids = set(entry["segment_id"] for entry in self.filtered_dataset) if self.filtered_dataset else set()

    for count, entry in enumerate(list(self.dataset), start=1):
      self.sm.update()
      if self.sm["deviceState"].started:
        break

      #self.dataset.remove(entry)

      start_coords = entry.get("start_coordinates")
      end_coords = entry.get("end_coordinates")
      if not start_coords or not end_coords:
        continue

      result = self.fetch_segments_from_overpass(start_coords, end_coords)
      if result is not None:
        segment_id, speed_limit = result

        if not segment_id:
          continue
        if speed_limit:
          continue
        if segment_id in existing_segment_ids:
          continue

        self.add_entry(self.filtered_dataset, {
          "segment_id": segment_id,
          "source": entry.get("source"),
          "speed_limit": entry.get("speed_limit"),
        })

        existing_segment_ids.add(segment_id)

      if count % 100 == 0:
        #params.put("SpeedLimits", json.dumps(list(self.dataset)))
        params.put("SpeedLimitsFiltered", json.dumps(list(deque(sorted(self.filtered_dataset, key=lambda entry: entry["segment_id"]), maxlen=MAX_ENTRIES))))

    #params.put("SpeedLimits", json.dumps(list(self.dataset)))
    params.put("SpeedLimitsFiltered", json.dumps(list(deque(sorted(self.filtered_dataset, key=lambda entry: entry["segment_id"]), maxlen=MAX_ENTRIES))))

    self.speed_limits_checked = True

def main():
  logger = MapSpeedLogger()

  while True:
    try:
      if not logger.speed_limits_checked and is_url_pingable("http://overpass-api.de"):
        logger.update_speed_limits()

      logger.log_speed_limit()
    except Exception as error:
      print(f"Error in speed_limit_filler: {error}")
      sentry.capture_exception(error)

if __name__ == "__main__":
  main()
