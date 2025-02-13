#!/usr/bin/env python3
import bisect
import json
import numpy as np

from sortedcontainers import SortedDict

from openpilot.common.conversions import Conversions as CV
from openpilot.common.realtime import DT_MDL
from openpilot.selfdrive.controls.lib.drive_helpers import V_CRUISE_MAX

from openpilot.selfdrive.frogpilot.frogpilot_variables import CRUISING_SPEED, PLANNER_TIME, params

CACHE_WINDOW = 1
CURVATURE_THRESHOLD = 1e-5
ROUNDING_PRECISION = 5
V_CRUISE_MAX_CONVERTED = V_CRUISE_MAX * CV.KPH_TO_MS

class SmartTurnSpeedController:
  def __init__(self, FrogPilotVCruise):
    self.frogpilot_planner = FrogPilotVCruise.frogpilot_planner

    self.data = SortedDict({entry["speed"]: [np.array([curve["curvature"], curve["lateral_accel"]]) for curve in entry.get("curvatures", [])] for entry in json.loads(params.get("UserCurvature") or "[]")})

    self.last_cached_speed = 0
    self.manual_long_timer = 0

    self.cached_entries = None
    self.cached_speeds = None

  def get_stsc_target(self, v_cruise, v_ego):
    self.update_cache(v_ego)
    if self.cached_speeds is None:
      return v_cruise

    road_curvature = round(self.frogpilot_planner.road_curvature, ROUNDING_PRECISION)
    closest_entry = min(self.cached_entries, key=lambda x: abs(x[0] - road_curvature))
    return np.clip((closest_entry[1] / closest_entry[0])**0.5, CRUISING_SPEED, v_cruise)

  def update_cache(self, v_ego):
    if not self.data:
      return

    if abs(self.last_cached_speed - v_ego) <= CACHE_WINDOW / 2:
      return

    data = {speed: entries[:] for speed, entries in self.data.items()}

    lower_bound = v_ego - CACHE_WINDOW
    upper_bound = v_ego + CACHE_WINDOW
    speeds_in_range = [speed for speed in data if lower_bound <= speed <= upper_bound]

    if not speeds_in_range:
      expansion_limit = min(v_ego - CRUISING_SPEED, V_CRUISE_MAX_CONVERTED - v_ego)
      increased_range = CACHE_WINDOW

      while increased_range <= expansion_limit:
        lower_bound = v_ego - CACHE_WINDOW - increased_range
        upper_bound = v_ego + CACHE_WINDOW + increased_range
        speeds_in_range = [speed for speed in data if lower_bound <= speed <= upper_bound]

        if speeds_in_range and any(speed < v_ego for speed in speeds_in_range) and any(speed > v_ego for speed in speeds_in_range):
          break

        increased_range += CACHE_WINDOW

    if speeds_in_range:
      min_speed = int(np.floor(min(speeds_in_range)))
      max_speed = int(np.ceil(max(speeds_in_range)))

      for speed in range(min_speed, max_speed + 1):
        if speed not in data:
          lower_idx = self.data.bisect_right(speed) - 1
          upper_idx = self.data.bisect_left(speed)

          if 0 <= lower_idx < len(self.data) and 0 <= upper_idx < len(self.data):
            lower_speed, lower_data = self.data.peekitem(lower_idx)
            upper_speed, upper_data = self.data.peekitem(upper_idx)

            if lower_speed != upper_speed:
              ratio = (speed - lower_speed) / (upper_speed - lower_speed)
              lower_curves = np.mean(lower_data, axis=0)
              upper_curves = np.mean(upper_data, axis=0)

              interpolated = [lower_curves + ratio * (upper_curves - lower_curves)]
              data[speed] = interpolated
              speeds_in_range.append(speed)

      sorted_speeds = sorted(speeds_in_range)
      self.cached_entries = np.vstack([data[speed] for speed in sorted_speeds])
      self.cached_speeds = np.array(sorted_speeds)
    else:
      self.cached_entries = None
      self.cached_speeds = None

    self.last_cached_speed = v_ego

  def update_curvature_data(self, v_ego):
    road_curvature = round(self.frogpilot_planner.road_curvature, ROUNDING_PRECISION)
    lateral_accel = round(v_ego**2 * road_curvature, ROUNDING_PRECISION)

    if abs(road_curvature) < CURVATURE_THRESHOLD or abs(lateral_accel) < 1:
      return

    entries = self.data.setdefault(v_ego, [])
    for i, entry in enumerate(entries):
      if abs(entry[0] - road_curvature) < CURVATURE_THRESHOLD:
        entries[i][1] = (entry[1] + lateral_accel) / 2
        return

    entries.append(np.array([road_curvature, lateral_accel]))

  def update(self, carControl, v_ego):
    if not carControl.longActive and V_CRUISE_MAX_CONVERTED >= v_ego > CRUISING_SPEED and not self.frogpilot_planner.tracking_lead:
      if self.manual_long_timer >= PLANNER_TIME:
        self.update_curvature_data(v_ego)
      self.manual_long_timer += DT_MDL

    elif self.manual_long_timer >= PLANNER_TIME:
      params.put_nonblocking("UserCurvature", json.dumps([
        {"speed": speed, "curvatures": [{"curvature": entry[0], "lateral_accel": entry[1]} for entry in entries]} for speed, entries in self.data.items()
      ]))
      self.manual_long_timer = 0

    else:
      self.manual_long_timer = 0
