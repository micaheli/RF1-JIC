//#define STM32F405xx
#include "includes.h"

volatile int retValChk;

//test
void FindCraftName(void);

int main(void)
{
	//Absolutely no MCU specific code to go here.

    int count = 16;

    //TODO: Make automatic
	retValChk = VectorIrqInit(ADDRESS_RFFW_START);

	//TODO Needs to pull parameters from flash here. For now we use defines
	retValChk = GetBoardHardwareDefs();

    retValChk = InitializeMCUSettings();

    retValChk = BoardInit();

    //DshotInit(1);
    
    retValChk = HandleRfbl();

    retValChk = LoadConfig(ADDRESS_CONFIG_START);

    //test
    FindCraftName();

    SpektrumBind(mainConfig.rcControlsConfig.bind);

    retValChk = HandleFcStartupReg();

    retValChk = InitBuzzer();
    retValChk = InitLeds();

    retValChk = InitUsb();

    retValChk = InitFlight(mainConfig.mixerConfig.escProtocol, mainConfig.mixerConfig.escUpdateFrequency);

    retValChk = InitQuopaMode();
    retValChk = InitDshotBeep();
    retValChk = InitDshotCommandState(); // send dshot commands and listen back if needed
    
    retValChk = InitWatchdog(WATCHDOG_TIMEOUT_32S);

    buzzerStatus.status = STATE_BUZZER_STARTUP;
    ledStatus.status    = LEDS_SLOW_BLINK;

    //DeInitActuators();
    //DelayMs(10);
    //InitializeGpio(ports[ACTUATOR2_GPIO], ACTUATOR2_PIN, 0);
    //inlineDigitalHi(ports[ACTUATOR2_GPIO], ACTUATOR2_PIN);
    //InitializeGpio(ports[ENUM_PORTB], GPIO_PIN_0, 0);

    while (1)
    {
        inlineDigitalHi(ports[ACTUATOR2_GPIO], ACTUATOR2_PIN);
    	Scheduler(count--);

    	if (count == -1)
    		count = 16;

		//If 1wire is run, the gyro is disabled for the SPMFCF400.  This prevents the watchdog from ever resetting, causing the board to reset.
	    if (oneWireHasRun)
		    FeedTheDog();
    }

}

//test
void FindCraftName(void)
{
    uint32_t craftNameFinder[7];

    uint32_t marker1 = 0x49555243;
    uint32_t marker2 = 0x21524553;
    uint32_t marker3 = 0x46F4F457;
    //mainConfig.marker1 = 0x43525549;
	//mainConfig.marker2 = 0x53455221;
    //mainConfig.marker3 = 0x574F4F46;
    //scan config area
    for (uint32_t wordOffset = 0x08010000; wordOffset < 0x08020000; wordOffset += 4)
    { //scan up to 1mb of flash starting at after rfbl //todo:set per mcu
		//flash goes like this. RFBL -> FW -> ESCs -> fade43f4 a62fe81a -> RFBL SIZE in 16 bit hex variable -> fade43f4 a62fe81a -> RFBL
		memcpy( &craftNameFinder, (char *)wordOffset, sizeof(craftNameFinder) );
        if ( (craftNameFinder[0] == marker1) && (craftNameFinder[1] == marker2) )
        { //RFBLM1 and 2 are different enfian of RFBLMR1 and 2
			memcpy( &foundCraftName, (char *)wordOffset+8, 16 );
		}
	}
	/*
	uint16_t rfblSize          = 0x0000;
	uint32_t rfblAddress       = 0x00000000;
	uint32_t wordOffset2       = 0x00000000;
	uint32_t addressFlashStart = ADDRESS_FLASH_START;
	uint32_t addressFlashEnd   = ADDRESS_FLASH_END;
	uint32_t addressRfblStart  = ADDRESS_RFBL_START;
	uint32_t firmwareFinderData[3];
	bool continueOn = false;
	FLASH_Status status = 0;
	uint32_t data32;
	int8_t attemptsRemaining = 5;
	//FADE 43F4  A62F E81A
	//F443 DEFA  1AE8 2FA6
	for (uint32_t wordOffset = addressFlashStart; wordOffset < addressFlashEnd; wordOffset += 4) { //scan up to 1mb of flash starting at after rfbl //todo:set per mcu
		//flash goes like this. RFBL -> FW -> ESCs -> fade43f4 a62fe81a -> RFBL SIZE in 16 bit hex variable -> fade43f4 a62fe81a -> RFBL
		memcpy( &firmwareFinderData, (char *) wordOffset, sizeof(firmwareFinderData) );
		if ( (firmwareFinderData[0] == RFBLMR1) && (firmwareFinderData[1] == RFBLMR2) ) { //RFBLM1 and 2 are different enfian of RFBLMR1 and 2
			rfblSize = (uint16_t)( ( (firmwareFinderData[2] & 0xFF) << 8) | ((firmwareFinderData[2] >> 8) & 0xFF)) ; //endian
			rfblAddress = wordOffset+20;
			memcpy( &firmwareFinderData, (char *) wordOffset+12, sizeof(firmwareFinderData) );
			if ( (firmwareFinderData[0] == RFBLMR1) && (firmwareFinderData[1] == RFBLMR2) ) {
				continueOn = true;
				break;
			}
		}
	}
	if (!continueOn) {
		return;
	}
	SKIP_GYRO=true;
	eraseRfbl(rfblSize);
	FLASH_Unlock();
	while (attemptsRemaining--) {
		FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);
		for (uint32_t wordOffset = rfblAddress; wordOffset < (rfblAddress + rfblSize); wordOffset += 4) {
			memcpy( &data32, (char *) wordOffset, sizeof(data32) ); //copy from new RFBL one word at a time
			status = FLASH_ProgramWord( addressRfblStart + wordOffset2, data32);
			wordOffset2 += 4;
			if (status != FLASH_COMPLETE) {
				break;
			}
		}
		if (status == FLASH_COMPLETE) {
			break;
		}
	}
	FLASH_Lock();
	SKIP_GYRO=false;
	rfblVersion = RFBL_VERSION;
	cfg1Version = CFG1_VERSION;
	*/
}