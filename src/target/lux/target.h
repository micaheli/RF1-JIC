#define	LUX

#define LEDn                    3

#define LED1_GPIO_Port          GPIOC
#define LED1_GPIO_Pin           GPIO_PIN_15

#define LED2_GPIO_Port          GPIOC
#define LED2_GPIO_Pin           GPIO_PIN_14

#define LED3_GPIO_Port          GPIOC
#define LED3_GPIO_Pin           GPIO_PIN_13

#define GYRO_SPI                SPI1
#define GYRO_SPI_CS_GPIO_Port   GPIOA
#define GYRO_SPI_CS_GPIO_Pin    GPIO_PIN_4
//#define GYRO_DMA_RX_IRQHandler  DMA2_Stream0_IRQHandler

#define GYRO_EXTI
#define GYRO_EXTI_GPIO_Port     GPIOA
#define GYRO_EXTI_GPIO_Pin      GPIO_PIN_5
#define GYRO_EXTI_IRQn          EXTI4_IRQn
#define GYRO_EXTI_IRQHandler    EXTI4_IRQHandler
