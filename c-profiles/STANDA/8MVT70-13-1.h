#ifndef __8MVT70_13_1
#define __8MVT70_13_1

#include <string.h>

#if defined(__APPLE__) && !defined(NOFRAMEWORK)
#include <libimported project/imported project.h>
#else
#include <imported project.h>
#endif


#define 8MVT70_13_1_BUILDER_VERSION_MAJOR  0
#define 8MVT70_13_1_BUILDER_VERSION_MINOR  10
#define 8MVT70_13_1_BUILDER_VERSION_BUGFIX 26
#define 8MVT70_13_1_BUILDER_VERSION_SUFFIX ""
#define 8MVT70_13_1_BUILDER_VERSION        "0.10.26"


#if defined(_MSC_VER)
#define inline __inline
#endif

static inline result_t imported project_set_profile_8MVT70_13_1(device_t id)
{
  result_t worst_result = result_ok;
  result_t result = result_ok;

  imported project_feedback_settings_t feedback_settings;
  memset((void*)&feedback_settings, 0, sizeof(imported project_feedback_settings_t));
  feedback_settings.IPS = 4000;
  feedback_settings.FeedbackType = IMPORTED PROJECT_FEEDBACK_NONE;
  feedback_settings.FeedbackFlags = IMPORTED PROJECT_FEEDBACK_ENC_TYPE_SINGLE_ENDED | IMPORTED PROJECT_FEEDBACK_ENC_TYPE_AUTO;
  feedback_settings.CountsPerTurn = 4000;
  result = imported project_set_feedback_settings(id, &feedback_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_home_settings_t home_settings;
  memset((void*)&home_settings, 0, sizeof(imported project_home_settings_t));
  home_settings.FastHome = 500;
  home_settings.uFastHome = 0;
  home_settings.SlowHome = 500;
  home_settings.uSlowHome = 0;
  home_settings.HomeDelta = 500;
  home_settings.uHomeDelta = 0;
  home_settings.HomeFlags = IMPORTED PROJECT_HOME_USE_FAST | IMPORTED PROJECT_HOME_STOP_SECOND_REV | IMPORTED PROJECT_HOME_STOP_FIRST_BITS | IMPORTED PROJECT_HOME_DIR_SECOND;
  result = imported project_set_home_settings(id, &home_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_move_settings_t move_settings;
  memset((void*)&move_settings, 0, sizeof(imported project_move_settings_t));
  move_settings.Speed = 1000;
  move_settings.uSpeed = 0;
  move_settings.Accel = 2000;
  move_settings.Decel = 4000;
  move_settings.AntiplaySpeed = 1000;
  move_settings.uAntiplaySpeed = 0;
  move_settings.MoveFlags = 0;
  result = imported project_set_move_settings(id, &move_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_engine_settings_t engine_settings;
  memset((void*)&engine_settings, 0, sizeof(imported project_engine_settings_t));
  engine_settings.NomVoltage = 0;
  engine_settings.NomCurrent = 1200;
  engine_settings.NomSpeed = 4000;
  engine_settings.uNomSpeed = 0;
  engine_settings.EngineFlags = IMPORTED PROJECT_ENGINE_LIMIT_RPM | IMPORTED PROJECT_ENGINE_ACCEL_ON | IMPORTED PROJECT_ENGINE_REVERSE;
  engine_settings.Antiplay = 575;
  engine_settings.MicrostepMode = IMPORTED PROJECT_MICROSTEP_MODE_FRAC_256;
  engine_settings.StepsPerRev = 200;
  result = imported project_set_engine_settings(id, &engine_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_entype_settings_t entype_settings;
  memset((void*)&entype_settings, 0, sizeof(imported project_entype_settings_t));
  entype_settings.EngineType = IMPORTED PROJECT_ENGINE_TYPE_STEP | IMPORTED PROJECT_ENGINE_TYPE_NONE;
  entype_settings.DriverType = IMPORTED PROJECT_DRIVER_TYPE_INTEGRATE;
  result = imported project_set_entype_settings(id, &entype_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_power_settings_t power_settings;
  memset((void*)&power_settings, 0, sizeof(imported project_power_settings_t));
  power_settings.HoldCurrent = 50;
  power_settings.CurrReductDelay = 1000;
  power_settings.PowerOffDelay = 60;
  power_settings.CurrentSetTime = 300;
  power_settings.PowerFlags = IMPORTED PROJECT_POWER_SMOOTH_CURRENT | IMPORTED PROJECT_POWER_OFF_ENABLED | IMPORTED PROJECT_POWER_REDUCT_ENABLED;
  result = imported project_set_power_settings(id, &power_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_secure_settings_t secure_settings;
  memset((void*)&secure_settings, 0, sizeof(imported project_secure_settings_t));
  secure_settings.LowUpwrOff = 800;
  secure_settings.CriticalIpwr = 4000;
  secure_settings.CriticalUpwr = 5000;
  secure_settings.CriticalT = 800;
  secure_settings.CriticalIusb = 450;
  secure_settings.CriticalUusb = 520;
  secure_settings.MinimumUusb = 420;
  secure_settings.Flags = IMPORTED PROJECT_ALARM_ENGINE_RESPONSE | IMPORTED PROJECT_ALARM_FLAGS_STICKING | IMPORTED PROJECT_ALARM_ON_BORDERS_SWAP_MISSET | IMPORTED PROJECT_H_BRIDGE_ALERT | IMPORTED PROJECT_ALARM_ON_DRIVER_OVERHEATING;
  result = imported project_set_secure_settings(id, &secure_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_edges_settings_t edges_settings;
  memset((void*)&edges_settings, 0, sizeof(imported project_edges_settings_t));
  edges_settings.BorderFlags = IMPORTED PROJECT_BORDER_STOP_RIGHT | IMPORTED PROJECT_BORDER_STOP_LEFT;
  edges_settings.EnderFlags = IMPORTED PROJECT_ENDER_SW2_ACTIVE_LOW | IMPORTED PROJECT_ENDER_SW1_ACTIVE_LOW | IMPORTED PROJECT_ENDER_SWAP;
  edges_settings.LeftBorder = -435;
  edges_settings.uLeftBorder = 0;
  edges_settings.RightBorder = 2035;
  edges_settings.uRightBorder = 0;
  result = imported project_set_edges_settings(id, &edges_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_pid_settings_t pid_settings;
  memset((void*)&pid_settings, 0, sizeof(imported project_pid_settings_t));
  pid_settings.KpU = 0;
  pid_settings.KiU = 0;
  pid_settings.KdU = 0;
  pid_settings.Kpf = 0.0036;
  pid_settings.Kif = 0.038;
  pid_settings.Kdf = 2.8e-05;
  result = imported project_set_pid_settings(id, &pid_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_sync_in_settings_t sync_in_settings;
  memset((void*)&sync_in_settings, 0, sizeof(imported project_sync_in_settings_t));
  sync_in_settings.SyncInFlags = 0;
  sync_in_settings.ClutterTime = 4;
  sync_in_settings.Position = 0;
  sync_in_settings.uPosition = 0;
  sync_in_settings.Speed = 0;
  sync_in_settings.uSpeed = 0;
  result = imported project_set_sync_in_settings(id, &sync_in_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_sync_out_settings_t sync_out_settings;
  memset((void*)&sync_out_settings, 0, sizeof(imported project_sync_out_settings_t));
  sync_out_settings.SyncOutFlags = IMPORTED PROJECT_SYNCOUT_ONSTOP | IMPORTED PROJECT_SYNCOUT_ONSTART;
  sync_out_settings.SyncOutPulseSteps = 100;
  sync_out_settings.SyncOutPeriod = 2000;
  sync_out_settings.Accuracy = 0;
  sync_out_settings.uAccuracy = 0;
  result = imported project_set_sync_out_settings(id, &sync_out_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_extio_settings_t extio_settings;
  memset((void*)&extio_settings, 0, sizeof(imported project_extio_settings_t));
  extio_settings.EXTIOSetupFlags = IMPORTED PROJECT_EXTIO_SETUP_OUTPUT;
  extio_settings.EXTIOModeFlags = IMPORTED PROJECT_EXTIO_SETUP_MODE_IN_STOP | IMPORTED PROJECT_EXTIO_SETUP_MODE_IN_NOP | IMPORTED PROJECT_EXTIO_SETUP_MODE_OUT_OFF;
  result = imported project_set_extio_settings(id, &extio_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_brake_settings_t brake_settings;
  memset((void*)&brake_settings, 0, sizeof(imported project_brake_settings_t));
  brake_settings.t1 = 300;
  brake_settings.t2 = 500;
  brake_settings.t3 = 300;
  brake_settings.t4 = 400;
  brake_settings.BrakeFlags = IMPORTED PROJECT_BRAKE_ENG_PWROFF;
  result = imported project_set_brake_settings(id, &brake_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_control_settings_t control_settings;
  memset((void*)&control_settings, 0, sizeof(imported project_control_settings_t));
  control_settings.MaxSpeed[0] = 100;
  control_settings.MaxSpeed[1] = 1000;
  control_settings.MaxSpeed[2] = 0;
  control_settings.MaxSpeed[3] = 0;
  control_settings.MaxSpeed[4] = 0;
  control_settings.MaxSpeed[5] = 0;
  control_settings.MaxSpeed[6] = 0;
  control_settings.MaxSpeed[7] = 0;
  control_settings.MaxSpeed[8] = 0;
  control_settings.MaxSpeed[9] = 0;
  const uint8_t control_settings_uMaxSpeed_temp[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(control_settings.uMaxSpeed, control_settings_uMaxSpeed_temp, sizeof(uint8_t) * 10);
  control_settings.Timeout[0] = 1000;
  control_settings.Timeout[1] = 1000;
  control_settings.Timeout[2] = 1000;
  control_settings.Timeout[3] = 1000;
  control_settings.Timeout[4] = 1000;
  control_settings.Timeout[5] = 1000;
  control_settings.Timeout[6] = 1000;
  control_settings.Timeout[7] = 1000;
  control_settings.Timeout[8] = 1000;
  control_settings.MaxClickTime = 300;
  control_settings.Flags = IMPORTED PROJECT_CONTROL_MODE_LR | IMPORTED PROJECT_CONTROL_MODE_OFF;
  control_settings.DeltaPosition = 1;
  control_settings.uDeltaPosition = 0;
  result = imported project_set_control_settings(id, &control_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_joystick_settings_t joystick_settings;
  memset((void*)&joystick_settings, 0, sizeof(imported project_joystick_settings_t));
  joystick_settings.JoyLowEnd = 0;
  joystick_settings.JoyCenter = 5000;
  joystick_settings.JoyHighEnd = 10000;
  joystick_settings.ExpFactor = 100;
  joystick_settings.DeadZone = 50;
  joystick_settings.JoyFlags = 0;
  result = imported project_set_joystick_settings(id, &joystick_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_ctp_settings_t ctp_settings;
  memset((void*)&ctp_settings, 0, sizeof(imported project_ctp_settings_t));
  ctp_settings.CTPMinError = 3;
  ctp_settings.CTPFlags = IMPORTED PROJECT_CTP_ERROR_CORRECTION;
  result = imported project_set_ctp_settings(id, &ctp_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_uart_settings_t uart_settings;
  memset((void*)&uart_settings, 0, sizeof(imported project_uart_settings_t));
  uart_settings.Speed = 115200;
  uart_settings.UARTSetupFlags = IMPORTED PROJECT_UART_PARITY_BIT_EVEN;
  result = imported project_set_uart_settings(id, &uart_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_network_settings_t network_settings;
  memset((void*)&network_settings, 0, sizeof(imported project_network_settings_t));
  network_settings.DHCPEnabled = 1;
  const uint8_t network_settings_IPv4Address_temp[4] = {255, 255, 255, 255};
  memcpy(network_settings.IPv4Address, network_settings_IPv4Address_temp, sizeof(uint8_t) * 4);
  const uint8_t network_settings_SubnetMask_temp[4] = {0, 0, 0, 0};
  memcpy(network_settings.SubnetMask, network_settings_SubnetMask_temp, sizeof(uint8_t) * 4);
  const uint8_t network_settings_DefaultGateway_temp[4] = {0, 0, 0, 0};
  memcpy(network_settings.DefaultGateway, network_settings_DefaultGateway_temp, sizeof(uint8_t) * 4);
  result = imported project_set_network_settings(id, &network_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_password_settings_t password_settings;
  memset((void*)&password_settings, 0, sizeof(imported project_password_settings_t));
  const int8_t password_settings_UserPassword_temp[20] = {48, 48, 48, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(password_settings.UserPassword, password_settings_UserPassword_temp, sizeof(int8_t) * 20);
  result = imported project_set_password_settings(id, &password_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_controller_name_t controller_name;
  memset((void*)&controller_name, 0, sizeof(imported project_controller_name_t));
  const int8_t controller_name_ControllerName_temp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(controller_name.ControllerName, controller_name_ControllerName_temp, sizeof(int8_t) * 16);
  controller_name.CtrlFlags = 0;
  result = imported project_set_controller_name(id, &controller_name);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_emf_settings_t emf_settings;
  memset((void*)&emf_settings, 0, sizeof(imported project_emf_settings_t));
  emf_settings.L = 0;
  emf_settings.R = 0;
  emf_settings.Km = 0;
  emf_settings.BackEMFFlags = IMPORTED PROJECT_BACK_EMF_KM_AUTO | IMPORTED PROJECT_BACK_EMF_RESISTANCE_AUTO | IMPORTED PROJECT_BACK_EMF_INDUCTANCE_AUTO;
  result = imported project_set_emf_settings(id, &emf_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_engine_advansed_setup_t engine_advansed_setup;
  memset((void*)&engine_advansed_setup, 0, sizeof(imported project_engine_advansed_setup_t));
  engine_advansed_setup.stepcloseloop_Kw = 50;
  engine_advansed_setup.stepcloseloop_Kp_low = 1000;
  engine_advansed_setup.stepcloseloop_Kp_high = 33;
  result = imported project_set_engine_advansed_setup(id, &engine_advansed_setup);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_extended_settings_t extended_settings;
  memset((void*)&extended_settings, 0, sizeof(imported project_extended_settings_t));
  extended_settings.Param1 = 0;
  result = imported project_set_extended_settings(id, &extended_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_stage_name_t stage_name;
  memset((void*)&stage_name, 0, sizeof(imported project_stage_name_t));
  const int8_t stage_name_PositionerName_temp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(stage_name.PositionerName, stage_name_PositionerName_temp, sizeof(int8_t) * 16);
  result = imported project_set_stage_name(id, &stage_name);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_stage_information_t stage_information;
  memset((void*)&stage_information, 0, sizeof(imported project_stage_information_t));
  const int8_t stage_information_Manufacturer_temp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(stage_information.Manufacturer, stage_information_Manufacturer_temp, sizeof(int8_t) * 16);
  const int8_t stage_information_PartNumber_temp[24] = {56, 77, 86, 84, 55, 48, 45, 49, 51, 45, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(stage_information.PartNumber, stage_information_PartNumber_temp, sizeof(int8_t) * 24);
  result = imported project_set_stage_information(id, &stage_information);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_stage_settings_t stage_settings;
  memset((void*)&stage_settings, 0, sizeof(imported project_stage_settings_t));
  stage_settings.LeadScrewPitch = 1;
  const int8_t stage_settings_Units_temp[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(stage_settings.Units, stage_settings_Units_temp, sizeof(int8_t) * 8);
  stage_settings.MaxSpeed = 20;
  stage_settings.TravelRange = 13;
  stage_settings.SupplyVoltageMin = 0;
  stage_settings.SupplyVoltageMax = 0;
  stage_settings.MaxCurrentConsumption = 0;
  stage_settings.HorizontalLoadCapacity = 0;
  stage_settings.VerticalLoadCapacity = 0;
  result = imported project_set_stage_settings(id, &stage_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_motor_information_t motor_information;
  memset((void*)&motor_information, 0, sizeof(imported project_motor_information_t));
  const int8_t motor_information_Manufacturer_temp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(motor_information.Manufacturer, motor_information_Manufacturer_temp, sizeof(int8_t) * 16);
  const int8_t motor_information_PartNumber_temp[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(motor_information.PartNumber, motor_information_PartNumber_temp, sizeof(int8_t) * 24);
  result = imported project_set_motor_information(id, &motor_information);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_motor_settings_t motor_settings;
  memset((void*)&motor_settings, 0, sizeof(imported project_motor_settings_t));
  motor_settings.MotorType = IMPORTED PROJECT_MOTOR_TYPE_UNKNOWN;
  motor_settings.ReservedField = 0;
  motor_settings.Poles = 0;
  motor_settings.Phases = 0;
  motor_settings.NominalVoltage = 0;
  motor_settings.NominalCurrent = 0;
  motor_settings.NominalSpeed = 0;
  motor_settings.NominalTorque = 0;
  motor_settings.NominalPower = 0;
  motor_settings.WindingResistance = 0;
  motor_settings.WindingInductance = 0;
  motor_settings.RotorInertia = 0;
  motor_settings.StallTorque = 0;
  motor_settings.DetentTorque = 0;
  motor_settings.TorqueConstant = 0;
  motor_settings.SpeedConstant = 0;
  motor_settings.SpeedTorqueGradient = 0;
  motor_settings.MechanicalTimeConstant = 0;
  motor_settings.MaxSpeed = 5000;
  motor_settings.MaxCurrent = 0;
  motor_settings.MaxCurrentTime = 0;
  motor_settings.NoLoadCurrent = 0;
  motor_settings.NoLoadSpeed = 0;
  result = imported project_set_motor_settings(id, &motor_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_encoder_information_t encoder_information;
  memset((void*)&encoder_information, 0, sizeof(imported project_encoder_information_t));
  const int8_t encoder_information_Manufacturer_temp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(encoder_information.Manufacturer, encoder_information_Manufacturer_temp, sizeof(int8_t) * 16);
  const int8_t encoder_information_PartNumber_temp[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(encoder_information.PartNumber, encoder_information_PartNumber_temp, sizeof(int8_t) * 24);
  result = imported project_set_encoder_information(id, &encoder_information);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_encoder_settings_t encoder_settings;
  memset((void*)&encoder_settings, 0, sizeof(imported project_encoder_settings_t));
  encoder_settings.MaxOperatingFrequency = 0;
  encoder_settings.SupplyVoltageMin = 0;
  encoder_settings.SupplyVoltageMax = 0;
  encoder_settings.MaxCurrentConsumption = 0;
  encoder_settings.PPR = 1000;
  encoder_settings.EncoderSettings = 0;
  result = imported project_set_encoder_settings(id, &encoder_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_hallsensor_information_t hallsensor_information;
  memset((void*)&hallsensor_information, 0, sizeof(imported project_hallsensor_information_t));
  const int8_t hallsensor_information_Manufacturer_temp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(hallsensor_information.Manufacturer, hallsensor_information_Manufacturer_temp, sizeof(int8_t) * 16);
  const int8_t hallsensor_information_PartNumber_temp[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(hallsensor_information.PartNumber, hallsensor_information_PartNumber_temp, sizeof(int8_t) * 24);
  result = imported project_set_hallsensor_information(id, &hallsensor_information);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_hallsensor_settings_t hallsensor_settings;
  memset((void*)&hallsensor_settings, 0, sizeof(imported project_hallsensor_settings_t));
  hallsensor_settings.MaxOperatingFrequency = 0;
  hallsensor_settings.SupplyVoltageMin = 0;
  hallsensor_settings.SupplyVoltageMax = 0;
  hallsensor_settings.MaxCurrentConsumption = 0;
  hallsensor_settings.PPR = 0;
  result = imported project_set_hallsensor_settings(id, &hallsensor_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_gear_information_t gear_information;
  memset((void*)&gear_information, 0, sizeof(imported project_gear_information_t));
  const int8_t gear_information_Manufacturer_temp[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(gear_information.Manufacturer, gear_information_Manufacturer_temp, sizeof(int8_t) * 16);
  const int8_t gear_information_PartNumber_temp[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(gear_information.PartNumber, gear_information_PartNumber_temp, sizeof(int8_t) * 24);
  result = imported project_set_gear_information(id, &gear_information);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_gear_settings_t gear_settings;
  memset((void*)&gear_settings, 0, sizeof(imported project_gear_settings_t));
  gear_settings.ReductionIn = 1;
  gear_settings.ReductionOut = 1;
  gear_settings.RatedInputTorque = 0;
  gear_settings.RatedInputSpeed = 0;
  gear_settings.MaxOutputBacklash = 0;
  gear_settings.InputInertia = 0;
  gear_settings.Efficiency = 0;
  result = imported project_set_gear_settings(id, &gear_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  imported project_accessories_settings_t accessories_settings;
  memset((void*)&accessories_settings, 0, sizeof(imported project_accessories_settings_t));
  const int8_t accessories_settings_MagneticBrakeInfo_temp[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(accessories_settings.MagneticBrakeInfo, accessories_settings_MagneticBrakeInfo_temp, sizeof(int8_t) * 24);
  accessories_settings.MBRatedVoltage = 0;
  accessories_settings.MBRatedCurrent = 0;
  accessories_settings.MBTorque = 0;
  accessories_settings.MBSettings = 0;
  const int8_t accessories_settings_TemperatureSensorInfo_temp[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  memcpy(accessories_settings.TemperatureSensorInfo, accessories_settings_TemperatureSensorInfo_temp, sizeof(int8_t) * 24);
  accessories_settings.TSMin = 0;
  accessories_settings.TSMax = 0;
  accessories_settings.TSGrad = 0;
  accessories_settings.TSSettings = IMPORTED PROJECT_TS_TYPE_THERMOCOUPLE | IMPORTED PROJECT_TS_TYPE_UNKNOWN;
  accessories_settings.LimitSwitchesSettings = 0;
  result = imported project_set_accessories_settings(id, &accessories_settings);

  if (result != result_ok)
  {
    if (worst_result == result_ok || worst_result == result_value_error)
    {
      worst_result = result;
    }
  }

  return worst_result;
}

#endif
