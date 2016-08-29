#define	REVO

#define LEDn                    2

#define LED1_GPIO_Port          GPIOB
#define LED1_GPIO_Pin           GPIO_PIN_4

#define LED2_GPIO_Port          GPIOB
#define LED2_GPIO_Pin           GPIO_PIN_5

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
#define RFFW_HID_PRODUCT_STRING "Revolution"
#define RFBL_HID_PRODUCT_STRING "Revolution RFBL"

//STM32F4 UID address
#define DEVICE_ID1                  0x1FFF7A10
#define DEVICE_ID2                  0x1FFF7A14
#define DEVICE_ID3                  0x1FFF7A18



#define HARDWARE_RFBL_PLUG
#define RFBL_GPIO1   GPIOA //Servo5
#define RFBL_PIN1    GPIO_PIN_1
#define RFBL_GPIO2   GPIOA //Servo6
#define RFBL_PIN2    GPIO_PIN_0
#define SPEK_GPIO    GPIOB
#define SPEK_PIN     GPIO_PIN_11
