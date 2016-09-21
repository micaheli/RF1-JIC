#define VRRACEF7
//TODO:
//MCU config
#define FC_PLLM	8
#define FC_PLLN	384
#define FC_PLLP	2
#define FC_PLLQ	8

#define SERVO1_PIN
//LED config
#define LEDn                    1

#define LED1_GPIO_Port          GPIOI
#define LED1_GPIO_Pin           GPIO_PIN_1


//Gyro config
#define GYRO_ALIGN              CW270

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
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_10
#define GYRO_EXTI_IRQn          TIM1_UP_TIM10_IRQn
#define GYRO_EXTI_IRQHandler    TIM1_UP_TIM10_IRQHandler

//USB config
#define RFFW_HID_PRODUCT_STRING "VRRACEF7"
#define RFBL_HID_PRODUCT_STRING "VRRACEF7 RFBL"

//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOA
#define VBUS_SENSING_PIN			GPIO_PIN_9

#define USE_USART1
#define USART1_RX_GPIO				GPIOB
#define USART1_RX_PIN				GPIO_PIN_7
#define USART1_TX_GPIO				GPIOA
#define USART1_TX_PIN				GPIO_PIN_6

#define USE_USART2
#define USART1_RX_GPIO				GPIOD
#define USART1_RX_PIN				GPIO_PIN_6
#define USART1_TX_GPIO				GPIOD
#define USART1_TX_PIN				GPIO_PIN_5

#define USE_USART3
#define USART3_RX_GPIO				GPIOD
#define USART3_RX_PIN				GPIO_PIN_9
#define USART3_TX_GPIO				GPIOD
#define USART3_TX_PIN				GPIO_PIN_8

#define USE_USART6
#define USART6_RX_GPIO				GPIOC
#define USART6_RX_PIN				GPIO_PIN_7
#define USART6_TX_GPIO				GPIOC
#define USART6_TX_PIN				GPIO_PIN_6

//RFBL config //TODO:
#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1   GPIOA //Servo5
#define RFBL_PIN1    GPIO_PIN_1
#define RFBL_GPIO2   GPIOA //Servo6
#define RFBL_PIN2    GPIO_PIN_0
#define SPEK_GPIO    GPIOB
#define SPEK_PIN     GPIO_PIN_11

#define USE_RFBL
#define ADDRESS_RFBL_START		(0x08000000)
#define ADDRESS_CONFIG_START	(0x0800C000)
#define ADDRESS_FLASH_START		(0x08020000)
#define ADDRESS_FLASH_END		(0x080FFFF0)

