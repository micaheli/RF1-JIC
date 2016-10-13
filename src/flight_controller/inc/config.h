#pragma once

#define CONFIG_VERSION (uint8_t)(2u)

typedef struct {
	rc_control_config rcControlsConfig;
	gyro_config       gyroConfig;
	mixer_config      mixerConfig;
	filter_device     filterConfig[AXIS_NUMBER];
	pid_terms         pidConfig[AXIS_NUMBER];
	uint8_t           version;
	uint16_t          size;
	uint8_t           czechsum;
} main_config;

extern main_config mainConfig;

void SaveConfig (uint32_t addresConfigStart);
uint8_t CalculateCzechsum(uint8_t *data, uint32_t length);
int ValidateConfig (uint32_t addresConfigStart);
void LoadConfig (uint32_t addresConfigStart);
void ResetConfig (uint32_t addresConfigStart);
