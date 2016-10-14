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



#define typeINT		0
#define typeUINT	1
#define typeFLOAT	2
#define typeSTRING	3


typedef struct {
    const char *name;
    const uint32_t type;
    void *ptr;
    float Min;
    float Max;
    float Default;
    const char *strDefault;
} config_variables_rec;

extern main_config mainConfig;
extern const config_variables_rec valueTable[];

void SaveConfig (uint32_t addresConfigStart);
uint8_t CalculateCzechsum(uint8_t *data, uint32_t length);
int ValidateConfig (uint32_t addresConfigStart);
void LoadConfig (uint32_t addresConfigStart);
void ResetConfig (uint32_t addresConfigStart);
void GenerateConfig(void);
