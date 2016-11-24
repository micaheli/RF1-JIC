#include "includes.h"

extern uint8_t tOutBuffer[];
extern uint8_t tInBuffer[];
uint32_t skipTaskHandlePcComm = 0;
uint32_t failsafeStage        = 0;
uint32_t autoSaveTimer        = 0;

void scheduler(int32_t count)
{

	switch (count) {

		case 0:
			taskHandlePcComm();
			break;
		case 1:
			taskLed();
			break;
		case 2:
			taskBuzzer();
			break;
		case 3:
			taskAutoSaveConfig();
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 10:
			break;
		case 11:
			break;
		case 12:
			break;
		default:
			break;

	}

}

inline void taskAutoSaveConfig(void) {
	autoSaveTimer = 0;
	return;
	if (!boardArmed) {
		if ( autoSaveTimer && ( InlineMillis() - autoSaveTimer > 1000) ) {
			autoSaveTimer = 0;
			SaveConfig(ADDRESS_CONFIG_START);
		}
	}
}

inline void taskHandlePcComm(void)
{
	if (skipTaskHandlePcComm)
		return;

	if (tOutBuffer[0]==2) { //we have a usb report

		ProcessCommand((char *)tOutBuffer);
		bzero(tOutBuffer, HID_EPIN_SIZE);

	}

}

inline void taskLed(void)
{
	static uint32_t lastUpdate = 0;
	UpdateLeds();

	if ( ( InlineMillis() - lastUpdate ) > 100 ) {
		lastUpdate = InlineMillis();
		ws2812_led_update(mainConfig.ledConfig.ledCount);
	}
}

inline void taskBuzzer(void)
{
	UpdateBuzzer();
}
