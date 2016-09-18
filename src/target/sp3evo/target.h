#define	SP3EVO

#define LEDn                    1

#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_8

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
#define DEVICE_ID1                  0x1FFFF7AC
#define DEVICE_ID2                  0x1FFFF7B0
#define DEVICE_ID3                  0x1FFFF7B4

