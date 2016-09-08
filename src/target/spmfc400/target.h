#define	SPMFC400


//MCU config
#define PLLM	8
#define PLLN	384
#define PLLP	2
#define PLLQ	8


//LED config
#define LEDn                    2

#define LED1_GPIO_Port          GPIOA
#define LED1_GPIO_Pin           GPIO_PIN_15

#define LED2_GPIO_Port          GPIOC
#define LED2_GPIO_Pin           GPIO_PIN_8


//Gyro config
#define GYRO_SPI                SPI2
#define GYRO_SPI_CS_GPIO_Port   GPIOB
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_12

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_4
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_DMA_TX_IRQn        DMA2_Stream3_IRQn //need to figure out
#define GYRO_DMA_TX_IRQHandler  DMA2_Stream3_IRQHandler //need to figure out
#define GYRO_DMA_RX_IRQn        DMA2_Stream0_IRQn //need to figure out
#define GYRO_DMA_RX_IRQHandler  DMA2_Stream0_IRQHandler //need to figure out

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_13
#define GYRO_EXTI_IRQn          EXTI4_IRQn //need to figure out
#define GYRO_EXTI_IRQHandler    EXTI4_IRQHandler //need to figure out


//USB config
#define RFFW_HID_PRODUCT_STRING "SPMFC400"
#define RFBL_HID_PRODUCT_STRING "SPMFC400 RFBL"


//STM32F4 UID address
#define DEVICE_ID1                  0x1FFF7A10
#define DEVICE_ID2                  0x1FFF7A14
#define DEVICE_ID3                  0x1FFF7A18


//RFBL config
#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1   GPIOA //usart 1 rx
#define RFBL_PIN1    GPIO_PIN_10
#define RFBL_GPIO2   GPIOA //usart 1 tx
#define RFBL_PIN2    GPIO_PIN_9
#define SPEK_GPIO    GPIOC
#define SPEK_PIN     GPIO_PIN_11

#define USE_RFBL
#define ADDRESS_RFBL_START		(0x08000000)
#define ADDRESS_CONFIG_START	(0x0800C000)
#define ADDRESS_FLASH_START		(0x08020000)
#define ADDRESS_FLASH_END		(0x0807FFF0)
