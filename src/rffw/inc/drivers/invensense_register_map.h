#pragma once

#define INVENS_RM_PRODUCT_ID    0x0C
#define INVENS_RM_CONFIG        0x1A
#define INVENS_RM_SMPLRT_DIV    0x19
#define INVENS_RM_GYRO_CONFIG   0x1B
#define INVENS_RM_ACCEL_CONFIG  0x1C
#define INVENS_RM_INT_PIN_CFG   0x37
#define INVENS_RM_INT_ENABLE    0x38
#define INVENS_RM_INT_STATUS    0x3A
#define INVENS_RM_GYRO_XOUT_H   0x43
#define INVENS_RM_USER_CTRL     0x6A
#define INVENS_RM_PWR_MGMT_1    0x6B
#define INVENS_RM_PWR_MGMT_2    0x6C
#define INVENS_RM_WHO_AM_I      0x75

// gyro config
#define INVENS_CONST_FSR_2000DPS 0x03
#define FCB_DISABLE             0x00
#define FCB_32_8800             0x01
#define FCB_32_3600             0x02

// accel conifg
#define INVENS_CONST_FSR_8G     0x03

// int pin config
#define INVENS_CONST_BYPASS_EN  0x02
#define INVENS_CONST_INT_RD_CLEAR 0x10

// int enable
#define INVENS_CONST_DATA_RDY_EN    0x01

// user_ctl
#define INVENS_CONST_I2C_IF_DIS 0x10
#define INVENS_CONST_FIFO_RESET 0x04
#define INVENS_CONST_SIG_COND_RESET 0x01

// pwm_mgmt_1
#define INVENS_CONST_H_RESET    0x80
#define INVENS_CONST_CLK_Z      0x03
#define INVENS_CONST_CLK_PLL    0x01
#define INVENS_CONST_TEMP_DIS   0x08
