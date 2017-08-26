#pragma once

enum
{
    DSHOT_CMD_MOTOR_STOP              = 0,
    DSHOT_CMD_BEEP1                   = 1,	// Wait length of beep plus 100ms before next command
    DSHOT_CMD_BEEP2                   = 2,	// Wait length of beep plus 100ms before next command
    DSHOT_CMD_BEEP3                   = 3,	// Wait length of beep plus 100ms before next command
    DSHOT_CMD_BEEP4                   = 4,	// Wait length of beep plus 100ms before next command
    DSHOT_CMD_BEEP5                   = 5,	// Wait length of beep plus 100ms before next command
    DSHOT_CMD_ESC_INFO                = 6,	// Currently not implemented
    DSHOT_CMD_SPIN_DIRECTION_1        = 7,	// Need 9x
    DSHOT_CMD_SPIN_DIRECTION_2        = 8,	// Need 9x
    DSHOT_CMD_3D_MODE_OFF             = 9,	// Need 9x
    DSHOT_CMD_3D_MODE_ON              = 10, // Need 9x
    DSHOT_CMD_SETTINGS_REQUEST        = 11, // Currently not implemented
    DSHOT_CMD_SAVE_SETTINGS           = 12, // Need 9x
    DSHOT_CMD_SPIN_DIRECTION_NORMAL   = 20,	// Need 9x
    DSHOT_CMD_SPIN_DIRECTION_REVERSED = 21,	// Need 9x
    DSHOT_CMD_LED0_ON                 = 22,	// Currently not implemented
    DSHOT_CMD_LED1_ON                 = 23,	// Currently not implemented
    DSHOT_CMD_LED2_ON                 = 24,	// Currently not implemented
    DSHOT_CMD_LED3_ON                 = 25,	// Currently not implemented
    DSHOT_CMD_LED0_OFF                = 26,	// Currently not implemented
    DSHOT_CMD_LED1_OFF                = 27,	// Currently not implemented
    DSHOT_CMD_LED2_OFF                = 28,	// Currently not implemented
    DSHOT_CMD_LED3_OFF                = 29,	// Currently not implemented
    DSHOT_CMD_MAX                     = 47,
};

extern int dShotFeedTheDog;

extern void DshotInit(int offlineMode);
