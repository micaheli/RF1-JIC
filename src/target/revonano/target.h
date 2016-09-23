#define	REVONANO


//MCU config
#define FC_PLLM	8
#define FC_PLLN	384
#define FC_PLLP	4
#define FC_PLLQ	8


//LED config
#define LEDn                    2

#define LED1_GPIO_Port          GPIOC
#define LED1_GPIO_Pin           GPIO_PIN_14

#define LED2_GPIO_Port          GPIOC
#define LED2_GPIO_Pin           GPIO_PIN_13

//buzzer setup
#define BUZZER_GPIO_Port        GPIOA
#define BUZZER_GPIO_Pin         GPIO_PIN_0

//Gyro config
#define GYRO_SPI                SPI2
#define GYRO_SPI_CS_GPIO_Port   GPIOB
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_12

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_4
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_DMA_TX_IRQn        DMA2_Stream3_IRQn       //todo: make correct
#define GYRO_DMA_TX_IRQHandler  DMA2_Stream3_IRQHandler //todo: make correct
#define GYRO_DMA_RX_IRQn        DMA2_Stream0_IRQn       //todo: make correct
#define GYRO_DMA_RX_IRQHandler  DMA2_Stream0_IRQHandler //todo: make correct

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOA
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_15
#define GYRO_EXTI_IRQn          EXTI4_IRQn         //todo: make correct
#define GYRO_EXTI_IRQHandler    EXTI4_IRQHandler   //todo: make correct


//USB config
#define RFFW_HID_PRODUCT_STRING "Revolution Nano"
#define RFBL_HID_PRODUCT_STRING "Revolution Nano RFBL"


//STM32F4 UID address
#define DEVICE_ID1					0x1FFF7A10
#define DEVICE_ID2					0x1FFF7A14
#define DEVICE_ID3					0x1FFF7A18


//Sbus inverter config
#define USE_SBUS_HARDWARE_INVERTER
#define SBUS_HARDWARE_GPIO			GPIOC
#define SBUS_HARDWARE_PIN			GPIO_PIN_15
#define SBUS_INVERTER_USART			USART2


//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOA
#define VBUS_SENSING_PIN			GPIO_PIN_9

#define USE_USART1
#define USART1_RX_GPIO				GPIOB
#define USART1_RX_PIN				GPIO_PIN_7
#define USART1_TX_GPIO				GPIOB
#define USART1_TX_PIN				GPIO_PIN_6

#define USE_USART2
#define USART2_RX_GPIO				GPIOA
#define USART2_RX_PIN				GPIO_PIN_3
#define USART2_TX_GPIO				GPIOA
#define USART2_TX_PIN				GPIO_PIN_2


//RFBL config
#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1					GPIOA //Servo5
#define RFBL_PIN1	 				GPIO_PIN_0
#define RFBL_GPIO2	 				GPIOA //Servo6
#define RFBL_PIN2	 				GPIO_PIN_1
#define SPEK_GPIO	 				GPIOB
#define SPEK_PIN	 				GPIO_PIN_6

#define USE_RFBL
#define ADDRESS_RFBL_START			(0x08000000)
#define ADDRESS_CONFIG_START		(0x0800C000)
#define ADDRESS_FLASH_START			(0x08020000)
#define ADDRESS_FLASH_END			(0x0807FFF0)
