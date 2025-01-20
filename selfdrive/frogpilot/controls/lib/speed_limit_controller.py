# PFEIFER - SLC - Modified by FrogAi for FrogPilot
#!/usr/bin/env python3
import json

from pathlib import Path

from openpilot.selfdrive.frogpilot.frogpilot_utilities import calculate_distance_to_point
from openpilot.selfdrive.frogpilot.frogpilot_variables import TO_RADIANS, params, params_memory

class SpeedLimitController:
  def __init__(self):
    self.experimental_mode = False
    self.speed_limit_changed = False

    self.desired_speed_limit = 0
    self.map_speed_limit = 0
    self.previous_dashboard_speed_limit = 0
    self.speed_limit = 0
    self.upcoming_speed_limit = 0

    self.source = "None"

    self.previous_position = None
    self.previous_speed_limit = params.get_float("PreviousSpeedLimit")

    self.speed_limit_file_path = Path("/data/media/speed_limits.json")
    self.speed_limit_file_path.parent.mkdir(parents=True, exist_ok=True)

  def update(self, dashboard_speed_limit, enabled, navigation_speed_limit, v_cruise, v_ego, frogpilot_toggles):
    self.update_map_speed_limit(v_ego, frogpilot_toggles)
    max_speed_limit = v_cruise if enabled else 0

    self.speed_limit = self.get_speed_limit(dashboard_speed_limit, max_speed_limit, navigation_speed_limit, frogpilot_toggles)
    self.desired_speed_limit = self.get_desired_speed_limit()

    self.experimental_mode = frogpilot_toggles.slc_fallback_experimental_mode and self.speed_limit == 0

    self.update_speed_limit_map(dashboard_speed_limit)

  def get_desired_speed_limit(self):
    if self.speed_limit > 1:
      if abs(self.speed_limit - self.previous_speed_limit) > 1:
        params.put_float_nonblocking("PreviousSpeedLimit", self.speed_limit)
        self.previous_speed_limit = self.speed_limit
        self.speed_limit_changed = True
      return self.speed_limit
    else:
      self.speed_limit_changed = False
      return 0

  def update_map_speed_limit(self, v_ego, frogpilot_toggles):
    position = json.loads(params_memory.get("LastGPSPosition") or "{}")
    if not position:
      self.map_speed_limit = 0
      return

    self.map_speed_limit = params_memory.get_float("MapSpeedLimit")

    next_map_speed_limit = json.loads(params_memory.get("NextMapSpeedLimit") or "{}")
    self.upcoming_speed_limit = next_map_speed_limit.get("speedlimit", 0)
    if self.upcoming_speed_limit > 1:
      current_latitude = position.get("latitude")
      current_longitude = position.get("longitude")

      upcoming_latitude = next_map_speed_limit.get("latitude")
      upcoming_longitude = next_map_speed_limit.get("longitude")

      distance_to_upcoming = calculate_distance_to_point(current_latitude * TO_RADIANS, current_longitude * TO_RADIANS, upcoming_latitude * TO_RADIANS, upcoming_longitude * TO_RADIANS)

      if self.previous_speed_limit < self.upcoming_speed_limit:
        max_distance = frogpilot_toggles.map_speed_lookahead_higher * v_ego
      else:
        max_distance = frogpilot_toggles.map_speed_lookahead_lower * v_ego

      if distance_to_upcoming < max_distance:
        self.map_speed_limit = self.upcoming_speed_limit

  def get_offset(self, speed_limit, frogpilot_toggles):
    if speed_limit < 13.5:
      return frogpilot_toggles.speed_limit_offset1
    if speed_limit < 24:
      return frogpilot_toggles.speed_limit_offset2
    if speed_limit < 29:
      return frogpilot_toggles.speed_limit_offset3
    return frogpilot_toggles.speed_limit_offset4

  def get_speed_limit(self, dashboard_speed_limit, max_speed_limit, navigation_speed_limit, frogpilot_toggles):
    limits = {
      "Dashboard": dashboard_speed_limit,
      "Map Data": self.map_speed_limit,
      "Navigation": navigation_speed_limit
    }
    filtered_limits = {source: float(limit) for source, limit in limits.items() if limit > 1}

    if filtered_limits:
      if frogpilot_toggles.speed_limit_priority_highest:
        self.source = max(filtered_limits, key=filtered_limits.get)
        return filtered_limits[self.source]

      if frogpilot_toggles.speed_limit_priority_lowest:
        self.source = min(filtered_limits, key=filtered_limits.get)
        return filtered_limits[self.source]

      for priority in [
        frogpilot_toggles.speed_limit_priority1,
        frogpilot_toggles.speed_limit_priority2,
        frogpilot_toggles.speed_limit_priority3
      ]:
        if priority is not None and priority in filtered_limits:
          self.source = priority
          return filtered_limits[priority]

    self.source = "None"

    if frogpilot_toggles.slc_fallback_previous_speed_limit:
      return self.previous_speed_limit

    if frogpilot_toggles.slc_fallback_set_speed:
      return max_speed_limit

    return 0

  def update_speed_limit_map(self, dashboard_speed_limit):
    if dashboard_speed_limit == self.previous_dashboard_speed_limit or dashboard_speed_limit == 0 or params_memory.get_float("MapSpeedLimit") != 0:
      return

    if self.previous_dashboard_speed_limit is not None and self.previous_position is not None:
      position = json.loads(params_memory.get("LastGPSPosition") or "{}")
      if not position:
        self.map_speed_limit = 0
        return
      current_latitude = position["latitude"]
      current_longitude = position["longitude"]

      zone = {
        "speed_limit": self.previous_dashboard_speed_limit,
        "start_latitude": self.previous_position.get("latitude"),
        "start_longitude": self.previous_position.get("longitude"),
        "end_latitude": current_latitude,
        "end_longitude": current_longitude,
      }

      try:
        with open(self.speed_limit_file_path, 'r') as f:
          data = json.load(f)
      except (FileNotFoundError, json.JSONDecodeError):
        data = []

      if zone not in data:
        data.append(zone)
        with open(self.speed_limit_file_path, 'w') as f:
          json.dump(data, f, indent=2)

      self.previous_dashboard_speed_limit = dashboard_speed_limit
      self.previous_position = position
