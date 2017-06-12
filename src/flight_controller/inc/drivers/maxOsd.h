#pragma once

#define OSD_SCREEN_BUFFER_SIZE 480    //max screen size in characters
#define OSD_EMPTY_CHAR ' '            //make sure these two don't match
#define OSD_EMPTY_CHAR_SHADOW_START 0 //make sure these two don't match

#define SCREEN_SIZE_PAL  480
#define SCREEN_SIZE_NTSC 390
#define VIDEO_LINES_PAL  16
#define VIDEO_LINES_NTSC 13

typedef enum
{
	OSD_COMMAND_STATUS_UNKNOWN         = -1,
	OSD_COMMAND_STATUS_IDLE            =  0,
	OSD_COMMAND_STATUS_SPI_SENDING_DMA =  1,
	OSD_COMMAND_STATUS_SPI_SENDING_IRQ =  2,
	OSD_COMMAND_STATUS_SPI_SENDING_BLK =  3,
	OSD_COMMAND_STATUS_SPI_SENDING_DMA_VIDEO_TYPE =  4,
} max_osd_command_status_t;

typedef enum
{
	OSD_STATUS_UNKNOWN          = -1,
	OSD_STATUS_DISABLED         =  0,
	OSD_STATUS_ENABLED          =  1,
	OSD_STATUS_SPI_RESETTING    =  2,
	OSD_STATUS_LOADING_CHAR_MAP =  3,
} max_osd_status_t;

typedef enum
{
	OSD_TYPE_UNKNOWN = -1,
	OSD_TYPE_SPI     =  0,
	OSD_TYPE_SERIAL  =  1,
} max_osd_type_t;

typedef enum
{
	VIDEO_MODE_UNKNOWN = -1,
	VIDEO_MODE_NTSC    = 0x00,
	VIDEO_MODE_PAL     = 0x40,
} video_mode_t;

typedef enum
{
	OSD_REQUEST_SCREEN_OFF = 0,
	OSD_REQUEST_SCREEN_1   = 1,
	OSD_REQUEST_SCREEN_2   = 2,
} osd_request_screen_t;

typedef struct
{
	max_osd_status_t         status;
	max_osd_command_status_t commandStatus;
	uint32_t                 lastComandMillis;
	osd_request_screen_t     requestScreen;
	max_osd_type_t           type;
	video_mode_t             videoMode;
	uint8_t                  screenBuffer[OSD_SCREEN_BUFFER_SIZE];
	uint8_t                  shadowBuffer[OSD_SCREEN_BUFFER_SIZE];
	int                      osdRows;
	int                      osdScreenSize;
} max_osd_record;

extern volatile max_osd_record maxOsdRecord;

extern int  InitMaxOsd(void);
extern void MaxOsdDmaTxCallback(uint32_t callbackNumber);
extern int  HandleMaxOsd(void);
extern void MaxOsdSetVidoOnOff(int on);