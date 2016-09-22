#define	LUX

//led config
#define LEDn                    3

#define LED1_GPIO_Port          GPIOC
#define LED1_GPIO_Pin           GPIO_PIN_15

#define LED2_GPIO_Port          GPIOC
#define LED2_GPIO_Pin           GPIO_PIN_14

#define LED3_GPIO_Port          GPIOC
#define LED3_GPIO_Pin           GPIO_PIN_13

//gyro config
#define GYRO_SPI                SPI1
#define GYRO_SPI_CS_GPIO_Port   GPIOA
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_4

// remapped SPI ports
#define GYRO_SPI_CLK_GPIO_Port  GPIOB
#define GYRO_SPI_CLK_GPIO_Pin   GPIO_PIN_3
#define GYRO_SPI_MISO_GPIO_Port GPIOB
#define GYRO_SPI_MISO_GPIO_Pin  GPIO_PIN_4
#define GYRO_SPI_MOSI_GPIO_Port GPIOB
#define GYRO_SPI_MOSI_GPIO_Pin  GPIO_PIN_5

#define GYRO_SPI_FAST_BAUD      SPI_BAUDRATEPRESCALER_2
#define GYRO_SPI_SLOW_BAUD      SPI_BAUDRATEPRESCALER_128

#define GYRO_DMA_TX_IRQn        DMA1_Channel3_IRQn
#define GYRO_DMA_TX_IRQHandler  DMA1_Channel3_IRQHandler
#define GYRO_DMA_RX_IRQn        DMA1_Channel2_IRQn
#define GYRO_DMA_RX_IRQHandler  DMA1_Channel2_IRQHandler

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOA
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_5
#define GYRO_EXTI_IRQn          EXTI9_5_IRQn
#define GYRO_EXTI_IRQHandler    EXTI9_5_IRQHandler

//BUZZER config
#define BUZZER_GPIO_Port        GPIOB
#define BUZZER_GPIO_Pin         GPIO_PIN_9


//USB config
#define RFFW_HID_PRODUCT_STRING "Lux"
#define RFBL_HID_PRODUCT_STRING "Lux RFBL"


//STM32F4 UID address
#define DEVICE_ID1                  0x1FFFF7AC
#define DEVICE_ID2                  0x1FFFF7B0
#define DEVICE_ID3                  0x1FFFF7B4


//RFBL config
#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1   GPIOA //Servo5
#define RFBL_PIN1    GPIO_PIN_15
#define RFBL_GPIO2   GPIOA //Servo6
#define RFBL_PIN2    GPIO_PIN_14
#define SPEK_GPIO    GPIOB
#define SPEK_PIN     GPIO_PIN_11

/*
0x08000000
0x08002000
0x08004000
0x08006000
0x08008000
0x0800A000
0x0800C000
0x0800E000
0x08010000 //128
0x08020000 //256
 */
#define USE_RFBL
#define ADDRESS_RFBL_START		(0x08000000)
#define ADDRESS_CONFIG_START	(0x0800A000)
#define ADDRESS_FLASH_START		(0x0800C000) //0x0800C000 - 0x08010000 - 0x08020000
#define ADDRESS_FLASH_END		(0x0801FFF0)
