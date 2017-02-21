#include "includes.h"

#include <stdio.h>

ledStatus_t ledStatus;

uint8_t colorChart[COLOR_CHART_SIZE][3] = {
		{255, 255, 255},
		{255, 255, 000},
		{255, 000, 000},
		{000, 255, 000},
		{000, 255, 255},
		{000, 000, 255},
		{255, 000, 255},
};

//todo: Do we want to init LEDs like this? Maybe an array is a better method
void InitLeds (void)
{
	int x;

	for (x=0;x<3;x++)
	{
		if (board.internalLeds[x].enabled)
		{
			InitializeGpio(ports[board.internalLeds[x].port], board.internalLeds[x].pin, 0);
		}
	}
	ledStatus.status = LEDS_OFF;
}


void DoLed(uint32_t number, uint32_t on)
{
	if (on)
	{
		if (board.internalLeds[number].enabled)
			HAL_GPIO_WritePin(ports[board.internalLeds[number].port], board.internalLeds[number].pin, GPIO_PIN_RESET);
	}
	else
	{
		if (board.internalLeds[number].enabled)
			HAL_GPIO_WritePin(ports[board.internalLeds[number].port], board.internalLeds[number].pin, GPIO_PIN_SET);
	}
}

void UpdateLeds(void)
{
	uint32_t timeNow = InlineMillis();

	if (ledStatus.status != ledStatus.lastStatus)
	{
		ledStatus.timeStart = timeNow;
	}
	ledStatus.lastStatus = ledStatus.status;

	switch (ledStatus.status)
	{
		case LEDS_OFF:
			DoLed(0, 0);
			DoLed(1, 0);
			DoLed(2, 0);
			break;
		case LEDS_ON:
			DoLed(0, 1);
			DoLed(1, 1);
			DoLed(2, 1);
			break;
		case LEDS_SLOW_BLINK:
			BlinkAllLeds(timeNow, 999, 1998);
			break;
		case LEDS_MED_BLINK:
			BlinkAllLeds(timeNow, 333, 666);
			break;
		case LEDS_FAST_BLINK:
			BlinkAllLeds(timeNow, 100, 200);
			break;
		case LEDS_FASTER_BLINK:
			BlinkAllLeds(timeNow, 25, 50);
			break;
		case LEDS_FASTEST_BLINK:
			BlinkAllLeds(timeNow, 10, 20);
			break;
		default:
			DoLed(0, 0);
			DoLed(1, 0);
			DoLed(2, 0);
			break;
	}
}

void BlinkAllLeds(uint32_t timeNow, uint16_t time1, uint16_t time2)
{
	if (((timeNow - ledStatus.timeStart) < time1) && (ledStatus.on) )
	{
		DoLed(0, 0);
		DoLed(1, 0);
		DoLed(2, 0);
		ledStatus.on = false;
	} else if (((timeNow - ledStatus.timeStart) > time1) && ((timeNow - ledStatus.timeStart) < time2) && (!ledStatus.on) )
	{
		DoLed(0, 1);
		DoLed(1, 1);
		DoLed(2, 1);
		ledStatus.on = true;
	} else if ((timeNow - ledStatus.timeStart) > time2 )
	{
		ledStatus.timeStart = timeNow;
	}
}

inline void UpdateWs2812Leds(void)
{
	uint8_t redAmount;
	uint8_t greenAmount;
	uint8_t blueAmount;
	uint8_t rgbArray[WS2812_MAX_LEDS*3];
	static uint8_t currentLedPulse = 0;
	static uint8_t colorPulse = 0;

	static uint32_t currentLedMode = 0;
	static uint32_t currentColorChart = 0;
	static uint32_t colorLatch = 0;
	static uint32_t modeLatch  = 0;
	static uint32_t updateInterval = 100;
	static uint32_t onceDone = 0;
	static uint32_t lastUpdate = 0;
	uint32_t x;
	uint32_t y;

	if (!ws2812LedRecord.enabled)
		return;

	//check color and mode change modes
	if ( ModeActive(M_LEDCOLOR) && !colorLatch )
	{
		colorLatch = 1;
		if (currentColorChart >= COLOR_CHART_SIZE)
		{
			currentColorChart = 0;
		}
		redAmount   = colorChart[currentColorChart++][0];
		greenAmount = colorChart[currentColorChart++][1];
		blueAmount  = colorChart[currentColorChart++][2];
	}
	else if (!ModeActive(M_LEDCOLOR))
	{
		colorLatch = 0;
	}

	if ( ModeActive(M_LEDMODE) && !modeLatch )
	{
		modeLatch = 1;
		currentLedMode ++;
		if (currentLedMode >= LED_MODE_END)
		{
			currentLedMode = 0;
		}
		mainConfig.ledConfig.ledMode = currentLedMode;
	}
	else if (!ModeActive(M_LEDMODE))
	{
		modeLatch = 0;
	}

	if ( ModeSet(M_BUZZER) && !onceDone)
	{
		onceDone = 1;
		currentLedMode = 0;
	}


	//Update WS2812 LEDs
	if ( ( InlineMillis() - lastUpdate ) > updateInterval )
	{
		lastUpdate = InlineMillis();

		mainConfig.ledConfig.ledCount = CONSTRAIN(mainConfig.ledConfig.ledCount, 1, WS2812_MAX_LEDS);

		switch (mainConfig.ledConfig.ledMode)
		{
			case LED_MODE_OFF:
				//leds off
				updateInterval = 250;
				redAmount      = 0;
				greenAmount    = 0;
				blueAmount     = 0;
				break;
			case LED_MODE_ON:
				//leds on
				updateInterval = 250;
				redAmount      = mainConfig.ledConfig.ledRed;
				greenAmount    = mainConfig.ledConfig.ledGreen;
				blueAmount     = mainConfig.ledConfig.ledBlue;
				break;
			case LED_MODE_DISO_FAST:
				//Disco Fast
				updateInterval = 20;
				redAmount      = (rand() & 0xF0) + 1;
				greenAmount    = (rand() & 0xF0) + 1;
				blueAmount     = (rand() & 0xF0) + 1;
				break;
			case LED_MODE_DISCO_SLOW:
				//Disco Slow
				updateInterval = 100;
				redAmount      = (rand() & 0xF0) + 1;
				greenAmount    = (rand() & 0xF0) + 1;
				blueAmount     = (rand() & 0xF0) + 1;
				break;
			case LED_MODE_PARTY_FAST:
				//Party Fast
				updateInterval = 20;
				redAmount   = (rand() & 0xF0) + 1;
				greenAmount = (rand() & 0xF0) + 1;
				blueAmount  = (rand() & 0xF0) + 1;

				if (redAmount > greenAmount)
					greenAmount = 0;
				if (redAmount > blueAmount)
					blueAmount = 0;

				if (blueAmount > greenAmount)
					greenAmount = 0;
				if (blueAmount > redAmount)
					redAmount = 0;

				if (greenAmount > blueAmount)
					blueAmount = 0;
				if (greenAmount > redAmount)
					blueAmount = 0;
				break;
			case LED_MODE_PARTY_SLOW:
				//Party Slow
				updateInterval = 100;
				redAmount   = (rand() & 0xF0) + 1;
				greenAmount = (rand() & 0xF0) + 1;
				blueAmount  = (rand() & 0xF0) + 1;

				if (redAmount > greenAmount)
					greenAmount = 0;
				if (redAmount > blueAmount)
					blueAmount = 0;

				if (blueAmount > greenAmount)
					greenAmount = 0;
				if (blueAmount > redAmount)
					redAmount = 0;

				if (greenAmount > blueAmount)
					blueAmount = 0;
				if (greenAmount > redAmount)
					blueAmount = 0;
				break;
			case LED_MODE_GYRO_MOTION:
				//Gyro motion Slow
				updateInterval = 5;

				redAmount   = (uint8_t)CONSTRAIN(ABS(filteredGyroData[YAW]),0,254)+1;
				greenAmount = (uint8_t)CONSTRAIN(ABS(filteredGyroData[ROLL]),0,254)+1;
				blueAmount  = (uint8_t)CONSTRAIN(ABS(filteredGyroData[PITCH]),0,254)+1;
				break;
			case LED_MODE_BATTERY_LEVEL:
				//Chnage color based on battery level
				updateInterval=20;
				if (averageVoltage > fullVoltage)
				{
					//make leds green
					greenAmount = 255;
					redAmount   = 0;
					blueAmount  = 0;
				}

				if (averageVoltage <= fullVoltage && averageVoltage> runningVoltage)
				{
					//make leds blue / green
					greenAmount = 200;
					redAmount   = 80;
					blueAmount  = 200;
				}
				if (averageVoltage <= runningVoltage && averageVoltage > lowVoltage)
				{
					//make leds yellow
					greenAmount=125;
					redAmount=124;
					blueAmount=0;
				}
				if (averageVoltage <= lowVoltage)
				{
					//make leds red
					greenAmount = 0;
					redAmount   = 255;
					blueAmount  = 0;
				}
				break;
			case LED_MODE_COLOR_PULSE:
				//Color Pulse
				updateInterval = 40;

				colorPulse += 1;

				if(colorPulse > 254)
				{
					currentLedPulse++;
					colorPulse = 0;
				}

				switch (currentLedPulse)
				{
					case 3:
						currentLedPulse = 0;
					case 0:
						redAmount   = colorPulse;
						greenAmount = 0;
						blueAmount  = 0;
						break;
					case 1:
						redAmount   = 0;
						greenAmount = colorPulse;
						blueAmount  = 0;
						break;
					case 2:
						redAmount   = 0;
						greenAmount = 0;
						blueAmount  = colorPulse;
						break;
				}
				break;
		}


		//fill the buffer
		y = 0;

		for (x = 0; x < mainConfig.ledConfig.ledCount; x++)
		{
			switch (mainConfig.ledConfig.ledMode)
			{
				case LED_MODE_MULTI_DISCO_FAST:
					//Disco Fast
					updateInterval = 20;
					redAmount      = (rand() & 0xF0) + 1;
					greenAmount    = (rand() & 0xF0) + 1;
					blueAmount     = (rand() & 0xF0) + 1;
					break;
				case LED_MODE_MULTI_DISCO_SLOW:
					//Disco Slow
					updateInterval = 100;
					redAmount      = (rand() & 0xF0) + 1;
					greenAmount    = (rand() & 0xF0) + 1;
					blueAmount     = (rand() & 0xF0) + 1;
					break;
				case LED_MODE_MULTI_PARTY_FAST:
					//Party Fast
					updateInterval = 20;
					redAmount   = (rand() & 0xF0) + 1;
					greenAmount = (rand() & 0xF0) + 1;
					blueAmount  = (rand() & 0xF0) + 1;

					if (redAmount > greenAmount)
						greenAmount = 0;
					if (redAmount > blueAmount)
						blueAmount = 0;

					if (blueAmount > greenAmount)
						greenAmount = 0;
					if (blueAmount > redAmount)
						redAmount = 0;

					if (greenAmount > blueAmount)
						blueAmount = 0;
					if (greenAmount > redAmount)
						blueAmount = 0;
					break;
				case LED_MODE_MULTI_PARTY_SLOW:
					//Party Slow
					updateInterval = 100;
					redAmount   = (rand() & 0xF0) + 1;
					greenAmount = (rand() & 0xF0) + 1;
					blueAmount  = (rand() & 0xF0) + 1;

					if (redAmount > greenAmount)
						greenAmount = 0;
					if (redAmount > blueAmount)
						blueAmount = 0;

					if (blueAmount > greenAmount)
						greenAmount = 0;
					if (blueAmount > redAmount)
						redAmount = 0;

					if (greenAmount > blueAmount)
						blueAmount = 0;
					if (greenAmount > redAmount)
						blueAmount = 0;
					break;
			}

			rgbArray[y++] = ~(uint8_t)greenAmount;
			rgbArray[y++] = ~(uint8_t)redAmount;
			rgbArray[y++] = ~(uint8_t)blueAmount;

		}

		OutputSerialDmaByte(rgbArray, y, ws2812LedRecord.ws2812Actuator, 1, 0, 0);

	}
}
