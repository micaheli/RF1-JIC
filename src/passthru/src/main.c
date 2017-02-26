/* Includes ------------------------------------------------------------------*/
#include "includes.h"

uint8_t tInBuffer[HID_EPIN_SIZE], tOutBuffer[HID_EPOUT_SIZE-1];

uint32_t StartSector = 0, EndSector = 0, Address = 0, i = 0 ;
__IO uint32_t data32 = 0 , MemoryProgramStatus = 0 ;
uint32_t toggle_led = 0;
int usbStarted = 0;
uint32_t ApplicationAddress = 0x08020000;
uint8_t bindSpektrum = 0;

//GPIO_TypeDef       *ports[11];
//serial_type         usarts[6];
//spi_type            spis[6];
//TIM_TypeDef        *timers[14];
//volatile uint32_t  *ccr[56];
//board_type          board;
//DMA_Stream_TypeDef *dmaStream[16];
//UART_HandleTypeDef  uartHandles[6];
//DMA_HandleTypeDef   dmaHandles[16];
//TIM_HandleTypeDef   pwmTimers[16];
//TIM_OC_InitTypeDef  sConfigOCHandles[16];
//SPI_HandleTypeDef   spiHandles[6];
//SPI_TypeDef        *spiInstance[6];

volatile function_pointer callbackFunctionArray[IRQH_FP_TOT];


void GyroExtiCallback (uint32_t callbackNumber)
{
	(void)(callbackNumber);
}

void GyroRxDmaCallback (uint32_t callbackNumber)
{
	(void)(callbackNumber);
}

void FlashDmaRxCallback (uint32_t callbackNumber)
{
	(void)(callbackNumber);
}


int main(void)
{

	VectorIrqInit(0x08060000);

	GetBoardHardwareDefs();
    InitializeMCUSettings();
    BoardInit();
    InitLeds();

	while(1)
	{

	}
}

void ErrorHandler(uint32_t error)
{
	(void)(error);
    while (1) {
        DoLed(1, 1);
        DoLed(2, 0);
        DelayMs(40);
        DoLed(1, 0);
        DoLed(2, 1);
        DelayMs(40);
    }
}
