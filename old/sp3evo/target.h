#define	SP3EVO
//led config
#define LEDn                    1

#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_8


//BUZZER config
#define BUZZER_GPIO_Port        GPIOC
#define BUZZER_GPIO_Pin         GPIO_PIN_15


#define GYRO_SPI                SPI1
#define GYRO_SPI_CS_GPIO_Port   GPIOA
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_4


//Serial Config
#define USE_VCP
#define VBUS_SENSING_GPIO			GPIOC
#define VBUS_SENSING_PIN			GPIO_PIN_5

#define USE_USART1
#define USART1_RX_GPIO				GPIOA
#define USART1_RX_PIN				GPIO_PIN_9
#define USART1_TX_GPIO				GPIOA
#define USART1_TX_PIN				GPIO_PIN_10

#define USE_USART2
#define USART2_RX_GPIO				GPIOA
#define USART2_RX_PIN				GPIO_PIN_15
#define USART2_TX_GPIO				GPIOA
#define USART2_TX_PIN				GPIO_PIN_14

#define USE_USART3
#define USART3_RX_GPIO				GPIOB
#define USART3RX_PIN				GPIO_PIN_11
#define USART3_TX_GPIO				GPIOB
#define USART3_TX_PIN				GPIO_PIN_10


// remapped SPI ports
#define GYRO_SPI_CLK_GPIO_Port  GPIOB
#define GYRO_SPI_CLK_GPIO_Pin   GPIO_PIN_3
#define GYRO_SPI_MISO_GPIO_Port GPIOB
#define GYRO_SPI_MISO_GPIO_Pin  GPIO_PIN_4
#define GYRO_SPI_MOSI_GPIO_Port GPIOB
#define GYRO_SPI_MOSI_GPIO_Pin  GPIO_PIN_5

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_2
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_DMA_TX_IRQn        DMA1_Channel3_IRQn			//todo: make correct
#define GYRO_DMA_TX_IRQHandler  DMA1_Channel3_IRQHandler	//todo: make correct
#define GYRO_DMA_RX_IRQn        DMA1_Channel2_IRQn			//todo: make correct
#define GYRO_DMA_RX_IRQHandler  DMA1_Channel2_IRQHandler	//todo: make correct

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOC
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_13
#define GYRO_EXTI_IRQn          EXTI15_10_IRQn
#define GYRO_EXTI_IRQHandler    EXTI15_10_IRQHandler


//USB config
#define RFFW_HID_PRODUCT_STRING "SP3 Evo"
#define RFBL_HID_PRODUCT_STRING "SP3 Evo RFBL"


//STM32F3 UID address
#define DEVICE_ID1              0x1FFFF7AC
#define DEVICE_ID2              0x1FFFF7B0
#define DEVICE_ID3              0x1FFFF7B4


//RFBL config
#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1   GPIOB
#define RFBL_PIN1    GPIO_PIN_11
#define RFBL_GPIO2   GPIOB
#define RFBL_PIN2    GPIO_PIN_10
#define SPEK_GPIO    GPIOB
#define SPEK_PIN     GPIO_PIN_11


#define USE_RFBL
#define ADDRESS_RFBL_START		(0x08000000)
#define ADDRESS_CONFIG_START	(0x0800A000)
#define ADDRESS_FLASH_START		(0x0800C000) //0x0800C000 - 0x08010000 - 0x08020000
#define ADDRESS_FLASH_END		(0x0801FFF0)
