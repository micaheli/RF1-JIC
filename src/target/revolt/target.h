#define	REVOLT


//MCU config
#define FC_PLLM	8
#define FC_PLLN	384
#define FC_PLLP	2
#define FC_PLLQ	8


//LED config
#define LEDn                    1

//#define LED1_GPIO_Port          GPIOB
//#define LED1_GPIO_Pin           GPIO_PIN_4

#define LED2_GPIO_Port          GPIOB
#define LED2_GPIO_Pin           GPIO_PIN_5

//BUZZER config
#define BUZZER_GPIO_Port        GPIOB
#define BUZZER_GPIO_Pin         GPIO_PIN_4


//Gyro Config

#define GYRO_SPI                SPI1
#define GYRO_SPI_CS_GPIO_Port   GPIOA
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_4

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_4
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_DMA_TX_IRQn        DMA2_Stream3_IRQn
#define GYRO_DMA_TX_IRQHandler  DMA2_Stream3_IRQHandler
#define GYRO_DMA_RX_IRQn        DMA2_Stream0_IRQn
#define GYRO_DMA_RX_IRQHandler  DMA2_Stream0_IRQHandler

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_4
#define GYRO_EXTI_IRQn          EXTI4_IRQn
#define GYRO_EXTI_IRQHandler    EXTI4_IRQHandler


//USB config
#define RFFW_HID_PRODUCT_STRING "RaceFlight Revolt"
#define RFBL_HID_PRODUCT_STRING "RaceFlight Revolt RFBL"


//STM32F4 UID address
#define DEVICE_ID1					0x1FFF7A10
#define DEVICE_ID2					0x1FFF7A14
#define DEVICE_ID3					0x1FFF7A18


//Sbus inverter config
#define USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			GPIOC
#define SBUS_HARDWARE_PIN			GPIO_PIN_0
#define SBUS_INVERTER_USART			USART1


//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOC
#define VBUS_SENSING_PIN			GPIO_PIN_5

#define USE_USART1
#define USART1_RX_GPIO				GPIOA
#define USART1_RX_PIN				GPIO_PIN_10
#define USART1_TX_GPIO				GPIOA
#define USART1_TX_PIN				GPIO_PIN_9

#define USE_USART3
#define USART3_RX_GPIO				GPIOB
#define USART3_RX_PIN				GPIO_PIN_11
#define USART3_TX_GPIO				GPIOB
#define USART3_TX_PIN				GPIO_PIN_10

#define USE_USART6
#define USART6_RX_GPIO				GPIOC
#define USART6_RX_PIN				GPIO_PIN_7
#define USART6_TX_GPIO				GPIOC
#define USART6_TX_PIN				GPIO_PIN_6


//RFBL config
#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1					GPIOC //USART 6 RX
#define RFBL_PIN1					GPIO_PIN_7
#define RFBL_GPIO2					GPIOC //USART 6 TX
#define RFBL_PIN2					GPIO_PIN_6
#define SPEK_GPIO					GPIOB //USART 3 RX
#define SPEK_PIN					GPIO_PIN_11

#define USE_RFBL
#define ADDRESS_RFBL_START			(0x08000000)
#define ADDRESS_CONFIG_START		(0x0800C000)
#define ADDRESS_FLASH_START			(0x08020000)
#define ADDRESS_FLASH_END			(0x080FFFF0)
