#include "includes.h"

void InitLeds(void)
{
    InitializeGpio(LED0_GPIO, LED0_PIN, 1);
	InitializeGpio(LED1_GPIO, LED1_PIN, 1);
	InitializeGpio(LED2_GPIO, LED2_PIN, 1);
}