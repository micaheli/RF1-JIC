#pragma once

#define CONFIG_VERSION			(uint32_t)(81U)
#define CONFIG_VERSION_STR		"81"
#define FIRMWARE_VERSION		"0.243.081 BETA" //RC18
//#define FIRMWARE_VERSION		"1.017.070 RC13"
#define FIRMWARE_NAME			"RaceFlight One"
#define FULL_VERSION_STRING		"#vr NAME:" FIRMWARE_NAME ";VERSION:" FIRMWARE_VERSION ";CONFIG:" CONFIG_VERSION_STR "\n"

#define RF_BUFFER_SIZE HID_EPIN_SIZE-1
#define FLIGHT_MODE_ARRAY_SIZE 96
// 32 flight modes listed from 0 to 31. first value is channel, second and third value is min and max

typedef struct
{
	rc_control_config rcControlsConfig;
	gyro_config       gyroConfig;
	mixer_config      mixerConfig;
	led_config		  ledConfig;
	telem_config	  telemConfig;
	filter_device     filterConfig[AXIS_NUMBER];
	pid_terms         pidConfig[AXIS_NUMBER];
	int16_t           flightModeArray[96];
	uint8_t           version;
	uint16_t          size;
	uint8_t           czechsum;
} main_config;

enum {typeINT=0,typeUINT,typeFLOAT,typeSTRING,};

typedef struct
{
    const char *name;
    const uint32_t type;
    const char *group;
    void  *ptr;
    float Min;
    float Max;
    float Default;
    const char *strDefault;
} config_variables_rec;

typedef struct
{
    const char *valueString;
    const int32_t valueInt;
} string_comp_rec;

extern char rf_custom_out_buffer[];

extern char *StripSpaces(char *inString);
extern char *CleanupString(char *inString);

extern uint32_t resetBoard;
extern main_config mainConfig;
extern const config_variables_rec valueTable[];

extern char   *CleanupNumberString(char *inString);
extern void    SaveConfig (uint32_t addresConfigStart);
extern void    LoadConfig (uint32_t addresConfigStart);
extern void    GenerateConfig(void);
extern void    ValidateConfigSettings(void);
extern void    ProcessCommand(char *inString);
extern int     RfCustomReply(char *rf_custom_out_buffer);
extern void    SendStatusReport(char *inString);
extern void    SaveAndSend(void);

extern int     RfCustomReplyBuffer(char *rfCustomSendBufferAdder);
extern int     SendRfCustomReplyBuffer(void);
