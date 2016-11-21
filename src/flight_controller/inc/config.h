#pragma once

#define CONFIG_VERSION			(uint8_t)(10U)
#define CONFIG_VERSION_STR		"10"
#define FIRMWARE_VERSION		"0.020 ALPHA"
#define FIRMWARE_NAME			"RaceFlight One"
#define FULL_VERSION_STRING		"NAME:" FIRMWARE_NAME ";VERSION:" FIRMWARE_VERSION ";CONFIG:" CONFIG_VERSION_STR "\0"

typedef struct {
	rc_control_config rcControlsConfig;
	gyro_config       gyroConfig;
	mixer_config      mixerConfig;
	led_config		  ledConfig;
	filter_device     filterConfig[AXIS_NUMBER];
	pid_terms         pidConfig[AXIS_NUMBER];
	uint8_t           version;
	uint16_t          size;
	uint8_t           czechsum;
} main_config;




enum {typeINT=0,typeUINT,typeFLOAT,typeSTRING,};

typedef struct {
    const char *name;
    const uint32_t type;
    const char *group;
    void *ptr;
    float Min;
    float Max;
    float Default;
    const char *strDefault;
} config_variables_rec;

extern main_config mainConfig;
extern const config_variables_rec valueTable[];

extern void SaveConfig (uint32_t addresConfigStart);
extern uint8_t CalculateCzechsum(const uint8_t *data, uint32_t length);
int ValidateConfig (uint32_t addresConfigStart);
extern void LoadConfig (uint32_t addresConfigStart);
extern void GenerateConfig(void);
extern void ProcessCommand(char *inString);
extern int RfCustomReply(char *rf_custom_out_buffer);

uint32_t CheckSafeMotors(uint32_t time, uint32_t deviationAllowed);
int32_t SetChannelMapAndDirection(uint32_t inChannel, uint32_t outChannel);
int32_t WhichInChannelChange(void);
void ResetChannelCheck(void);
int32_t FindRxMinMax(void);
int32_t FindRxCenter(void);
