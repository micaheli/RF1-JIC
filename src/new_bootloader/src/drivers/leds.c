#include "includes.h"

ledStatus_t ledStatus;

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

void StartupBlink (uint16_t blinks, uint32_t delay)
{

	uint32_t a, ledToggle;

	//Startup Blink
	ledToggle = 0;
	DoLed(0, 1);
	DoLed(1, 0);
	for( a = 0; a < blinks; a = a + 1 )
	{ //fast blink
		if (ledToggle)
		{
			ledToggle=0;
			DoLed(0, 1);
			DoLed(1, 0);
		}
		else
		{
			ledToggle=1;
			DoLed(0, 0);
			DoLed(1, 1);
		}
		DelayMs(delay);
	}
	DoLed(0, 0);
	DoLed(1, 0);

}

void ErrorBlink(void)
{
	uint32_t x;

	for (x = 0; x < 100; x += 10)
	{
		StartupBlink(10, x);
	}
}

void DoneFlashBlink(void)
{
	int32_t x;

	for (x = 50; x >= 0; x -= 2)
	{
		StartupBlink(2, x);
	}
}

void CoolLedEffect(uint32_t pwmPeriod, uint32_t dutyNumber, uint32_t ledNumber)
{
	//pwm an LED based on Micros()
	uint32_t currentTime;
	uint32_t currentDutyIn;
	uint32_t pwmOffAt;

	currentTime   = Micros();
	currentDutyIn = (currentTime % pwmPeriod);
	pwmOffAt      = currentTime - currentDutyIn + dutyNumber;

	if (currentTime > pwmOffAt)
	{
		DoLed(ledNumber, 1);
	}
	else
	{
		DoLed(ledNumber, 0);
	}
}
