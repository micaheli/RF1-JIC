#pragma once

rc_control_config rcControlsConfig;
gyro_config gyroConfig;
filter_device filterConfig[AXIS_NUMBER];
pid_terms pidConfig[AXIS_NUMBER];

void LoadConfig (void);
