from cereal import car
from enum import Enum
from dataclasses import dataclass
from openpilot.common.numpy_fast import clip, interp
from typing import Any, Literal
from cereal.messaging import SubMaster
from openpilot.common.realtime import DT_CTRL
from openpilot.common.params import Params
from openpilot.selfdrive.controls.lib.longcontrol import LongControl
from openpilot.selfdrive.controls.lib.longitudinal_mpc_lib.long_mpc import LongitudinalMpc, STOP_DISTANCE
from openpilot.selfdrive.car.hyundai.values import HyundaiFlags, CarControllerParams

class LongitudinalMode(str, Enum):
    ACC = 'acc'
    E2E = 'e2e'

@dataclass
class TuningConstants:
    BRAKE_ONSET_DISTANCE: float = 25.0
    MODE_TRANSITION_DURATION: float = 3.5
    DISTANCE_TO_STOP_THRESHOLD: float = 4.0
    BRAKE_DISTANCE_SCALE: float = 1.15
    BRAKE_START_SPEED: float = 1.0

RADAR_TRACKS = ['radarState', 'modelV2']
LongCtrlState = car.CarControl.Actuators.LongControlState


class HKGLongitudinalTuning:
  def __init__(self,CP) -> None:
    self.CP = CP
    self.config = TuningConstants()
    self._setup_controllers()
    self._init_state()

  def _setup_controllers(self) -> None:
    """Initialize controllers and messaging"""
    self.mpc = LongitudinalMpc(mode='acc')
    self.long_control = LongControl(self.CP)
    self.DT_CTRL = DT_CTRL
    self.params = Params()
    self.hkg_tuning = self.params.get_bool('HKGtuning')
    self.sm = SubMaster(RADAR_TRACKS)

  def _init_state(self) -> None:
    """Initialize control state variables"""
    self.current_mode = LongitudinalMode.ACC
    self.e2e_enabled = False
    self.mode_transition_timer = 0.0
    self.last_accel = 0.0
    self.brake_ramp = 0.0
    self.brake_ratio = 0.0
    self.brake_aggressiveness = 0.0
    self.ramp_rate = 0.0
    self.accel_last = 0.0


  def reset(self) -> None:
    self.mpc.reset()
    self.long_control.reset()

  def set_mode(self, mode: Literal[LongitudinalMode.ACC, LongitudinalMode.E2E]) -> None:
    """Set longitudinal control mode."""
    if mode != self.current_mode:
        self.mode_transition_timer = 0.0
    self.current_mode = mode
    self.e2e_enabled = (mode == LongitudinalMode.E2E)
    self.mpc.mode = 'e2e' if mode == LongitudinalMode.E2E else 'acc'

  def get_mode_blend_factor(self) -> float:
    """Calculate the blending factor during mode transitions."""
    if self.mode_transition_timer < self.config.MODE_TRANSITION_DURATION:
        self.mode_transition_timer += DT_CTRL
        blend = interp(self.mode_transition_timer,
                        [0.0, self.config.MODE_TRANSITION_DURATION],
                        [0.0, 1.0])
        return clip(blend, 0.0, 1.0)
    return 1.0

  def _get_lead_distance(self) -> float:
    """Get closest lead distance from radar or vision."""
    lead_distance = float('inf')

    # Check radar lead
    lead = self.sm['radarState'].leadOne
    if lead.status:
        lead_distance = min(lead_distance, lead.dRel)

    # Check vision lead
    if len(self.sm['modelV2'].leadsV3) > 0:
        vision_lead = self.sm['modelV2'].leadsV3[0]
        if len(vision_lead.x) > 0 and vision_lead.prob > 0.5:
            lead_distance = min(lead_distance, float(vision_lead.x[0]))

    return lead_distance

  def update(self, accel, CS, clip,
             mode: Literal[LongitudinalMode.ACC, LongitudinalMode.E2E] = LongitudinalMode.ACC) -> float:
    """Update longitudinal control."""
    if mode != self.current_mode:
      self.set_mode(mode)
      if self.current_mode == LongitudinalMode.E2E and accel < 0:
        accel = accel * 0.6

    # Get lead distance and accel limits
    lead_distance = self._get_lead_distance()

    # Force minimum stopping distance
    min_stopping_distance = STOP_DISTANCE + 1.5
    if mode == LongitudinalMode.ACC:
      if lead_distance < min_stopping_distance:
        target_decel = interp(lead_distance,
                                 [min_stopping_distance - 1.0, min_stopping_distance],
                                 [-2.0, -1.0])
        accel = max(target_decel, accel)

      lead_distance = max(lead_distance, min_stopping_distance)

    # Determine brake factor based on lead distance
    brake_factor = 1.0
    if lead_distance < self.config.BRAKE_ONSET_DISTANCE:
      brake_factor = interp(lead_distance,
                              [self.config.DISTANCE_TO_STOP_THRESHOLD, self.config.BRAKE_ONSET_DISTANCE],
                              [0.65, 1.0])

    # Brake smoothing when brake is pressed or at low speeds
    if CS.out.brakePressed or CS.out.vEgo < self.config.BRAKE_START_SPEED:
      brake_factor *= 0.9

    # Apply brake smoothing to negative accelerations
    if accel < 0:
      accel *= brake_factor

    blend = self.get_mode_blend_factor()
    if self.current_mode == LongitudinalMode.E2E and accel < 0:
      accel *= interp(blend, [0.0, 0.3, 0.7, 1.0], [0.65, 0.75, 0.85, 1.0])

    accel_delta = accel - self.accel_last

    ramp_rate = 0.7

    # Handling for transitions into braking
    if accel < 0:
      brake_ratio = clip(abs(accel / CarControllerParams.ACCEL_MIN), 0.0, 1.0)
      brake_aggressiveness = brake_ratio ** 2.0

      if self.accel_last >= 0:
        accel *= 0.8
        self.brake_ramp = 0.0

      ramp_rate = interp(brake_aggressiveness,
                        [0.0, 0.2, 0.4, 0.6, 0.8, 1.0],
                        [0.15, 0.25, 0.35, 0.45, 0.6, 0.8])

      if brake_ratio > 0.8:
        ramp_rate *= 0.8

      if self.accel_last >= 0:
        self.brake_ramp = 0.0
        ramp_rate *= 0.5

    self.brake_ramp = min(1.0, self.brake_ramp + (ramp_rate * self.DT_CTRL))

    abs_accel = clip(abs(accel), 0.0, 2.0)
    smooth_factor = interp(abs_accel,
                          [0.0, 0.3, 0.6, 1.0, 1.5, 2.0],  # breakpoints
                          [0.95, 0.85, 0.75, 0.65, 0.55, 0.45])  # Smoothing factors

    if accel < 0 and brake_aggressiveness > 0.8:
      smooth_factor *= 0.8

    if abs_accel < 0.5:
      smooth_factor *= interp(abs_accel,
                              [0.0, 0.5],
                              [1.3, 1.0])

    accel_delta *= (smooth_factor * self.brake_ramp)

    if abs(accel_delta) > 0.5:  # For large mph changes
      damping = interp(abs(accel_delta),
                      [0.5, 1.0, 1.5, 2.0],
                      [0.90, 0.80, 0.70, 0.60])
      accel_delta *= damping

    limited_accel_delta = accel_delta

    if accel < 0 and self.accel_last > 0:
      transition_factor = interp(abs(accel),
                               [0, 0.5, 1.0, 1.5, 2.0],
                               [0.3, 0.4, 0.5, 0.6, 0.7])
      limited_accel_delta *= transition_factor

    accel = self.accel_last + limited_accel_delta
    self.accel_last = accel

    return accel

  def compute_jerk(self, accel: float, vEgo: float, normal_jerk: float) -> float:
    """Compute jerk from standstill to 3m/s"""
    if accel > 0:
        return interp(vEgo, [0, 3], [0.5, normal_jerk])
    return normal_jerk

  def apply_tune(self, CP: Any) -> None:
    """Apply base tuning parameters to CarParams."""
    CP.vEgoStopping = 0.3
    CP.vEgoStarting = 0.1
    CP.stoppingDecelRate = 0.01
    CP.startAccel = 1.0 if bool(CP.flags & (HyundaiFlags.HYBRID | HyundaiFlags.EV)) else 1.6
    CP.startingState = True


class HKGLongitudinalController:
    def __init__(self, CP):
        if Params().get_bool("HKGtuning"):
            self.tuning = HKGLongitudinalTuning(CP)
        else:
            self.tuning = None

    def apply_tune(self, CP):
        if self.tuning:
            self.tuning.apply_tune(CP)

    def update(self, accel, CS, clip, mode=LongitudinalMode.ACC):
        if self.tuning:
            return self.tuning.update(accel, CS, clip, mode)
        return accel

    def compute_jerk(self, accel: float, vEgo: float, normal_jerk: float) -> float:
      return self.tuning.compute_jerk(accel, vEgo, normal_jerk)
