#!/usr/bin/env python3
import json
import numpy as np

from sortedcontainers import SortedDict

from openpilot.common.conversions import Conversions as CV
from openpilot.common.numpy_fast import clip
from openpilot.common.realtime import DT_MDL
from openpilot.selfdrive.controls.lib.drive_helpers import V_CRUISE_MAX

from openpilot.selfdrive.frogpilot.frogpilot_variables import CRUISING_SPEED, PLANNER_TIME, params

CACHE_WINDOW = 1
CURVATURE_THRESHOLD = 1e-10
ROUNDING_PRECISION = 10

class SmartTurnSpeedController:
  def __init__(self, FrogPilotVCruise):
    self.frogpilot_vcruise = FrogPilotVCruise

    self.data = SortedDict({entry["speed"]: [np.array([curve["curvature"], curve["lateral_accel"]]) for curve in entry.get("curvatures", [])] for entry in json.loads(params.get("UserCurvature") or "[]")})

    self.last_cached_speed = 0
    self.manual_long_timer = 0
    self.stsc_target = 0

    self.cached_entries = None
    self.cached_speeds = None

  def update_cache(self, v_ego):
    if abs(self.last_cached_speed - v_ego) <= CACHE_WINDOW / 2:
      return

    speeds_in_range = list(self.data.irange(v_ego - CACHE_WINDOW, v_ego + CACHE_WINDOW))
    if speeds_in_range:
      self.cached_entries = np.vstack([self.data[speed] for speed in speeds_in_range])
      self.cached_speeds = np.array(speeds_in_range)
    else:
      self.cached_entries = self.cached_speeds = None

    self.last_cached_speed = v_ego

  def set_stsc_target(self, carControl, v_cruise, v_ego):
    if not self.data or v_ego < CRUISING_SPEED or not carControl.longActive:
      self.stsc_target = v_cruise
      return

    self.update_cache(v_ego)
    if self.cached_speeds is None:
      self.stsc_target = v_cruise
      return

    road_curvature = round(self.frogpilot_vcruise.frogpilot_planner.road_curvature, ROUNDING_PRECISION)
    closest_entry = min(self.cached_entries, key=lambda x: abs(x[0] - road_curvature))
    self.stsc_target = clip((closest_entry[1] / closest_entry[0])**0.5, CRUISING_SPEED, v_cruise)

  def update_curvature_data(self, v_ego):
    road_curvature = round(self.frogpilot_vcruise.frogpilot_planner.road_curvature, ROUNDING_PRECISION)
    lateral_accel = round(v_ego**2 * road_curvature, ROUNDING_PRECISION)

    if abs(road_curvature) < CURVATURE_THRESHOLD or abs(lateral_accel) < 1:
      return

    entries = self.data.setdefault(v_ego, [])
    for i, entry in enumerate(entries):
      if abs(entry[0] - road_curvature) < CURVATURE_THRESHOLD:
        entries[i][1] = (entry[1] + lateral_accel) / 2
        return

    entries.append(np.array([road_curvature, lateral_accel]))

  def update(self, carControl, v_cruise, v_ego):
    if not carControl.longActive and V_CRUISE_MAX * CV.KPH_TO_MS >= v_ego > CRUISING_SPEED and not self.frogpilot_vcruise.frogpilot_planner.tracking_lead:
      if self.manual_long_timer >= PLANNER_TIME:
        self.update_curvature_data(v_ego)
      self.manual_long_timer += DT_MDL

    elif self.manual_long_timer >= PLANNER_TIME:
      params.put_nonblocking("UserCurvature", json.dumps([
        {"speed": speed, "curvatures": [{"curvature": entry[0], "lateral_accel": entry[1]} for entry in entries]}
        for speed, entries in self.data.items()
      ]))
      self.manual_long_timer = 0

    else:
      self.set_stsc_target(carControl, v_cruise, v_ego)
      self.manual_long_timer = 0
