#include "includes.h"


// use variable record but instead of storing address of variable, store offset based on address of field, that way it works with the record loaded from file

uint32_t rfCustomReplyBufferPointer = 0;
uint32_t rfCustomReplyBufferPointerSent = 0;

#define LARGE_RF_BUFFER_SIZE 4096
main_config mainConfig;
uint32_t resetBoard = 0;
char rf_custom_out_buffer[RF_BUFFER_SIZE];
char rfCustomSendBufferAdder[RF_BUFFER_SIZE];
char rfCustomSendBuffer[LARGE_RF_BUFFER_SIZE];
unsigned char rfReplyBuffer[HID_EPIN_SIZE];

static uint32_t ValidateConfig (uint32_t addresConfigStart);
static void     SetValueOrString(uint32_t position, char *value);
static void     SetValue(uint32_t position, char *value);
static void     DlflStatusDump(void);
static int32_t  GetValueFromString(char *string, const string_comp_rec thisStringCompTable[], uint32_t sizeOfArray);

//static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


const string_comp_rec vtxStringCompTable[] = {
		//telemetry.h.h
		{"a1", VTX_CH_A1 },
		{"a2", VTX_CH_A2 },
		{"a3", VTX_CH_A3 },
		{"a4", VTX_CH_A4 },
		{"a5", VTX_CH_A5 },
		{"a6", VTX_CH_A6 },
		{"a7", VTX_CH_A7 },
		{"a8", VTX_CH_A8 },

		{"b1", VTX_CH_B1 },
		{"b2", VTX_CH_B2 },
		{"b3", VTX_CH_B3 },
		{"b4", VTX_CH_B4 },
		{"b5", VTX_CH_B5 },
		{"b6", VTX_CH_B6 },
		{"b7", VTX_CH_B7 },
		{"b8", VTX_CH_B8 },

		{"e1", VTX_CH_E1 },
		{"e2", VTX_CH_E2 },
		{"e3", VTX_CH_E3 },
		{"e4", VTX_CH_E4 },
		{"e5", VTX_CH_E5 },
		{"e6", VTX_CH_E6 },
		{"e7", VTX_CH_E7 },
		{"e8", VTX_CH_E8 },

		{"f1", VTX_CH_F1 },
		{"f2", VTX_CH_F2 },
		{"f3", VTX_CH_F3 },
		{"f4", VTX_CH_F4 },
		{"f5", VTX_CH_F5 },
		{"f6", VTX_CH_F6 },
		{"f7", VTX_CH_F7 },
		{"f8", VTX_CH_F8 },

		{"r1", VTX_CH_R1 },
		{"r2", VTX_CH_R2 },
		{"r3", VTX_CH_R3 },
		{"r4", VTX_CH_R4 },
		{"r5", VTX_CH_R5 },
		{"r6", VTX_CH_R6 },
		{"r7", VTX_CH_R7 },
		{"r8", VTX_CH_R8 },

};

const string_comp_rec stringCompTable[] = {
		//mixer.h
		{"ESC_MULTISHOT",    ESC_MULTISHOT },
		{"ESC_ONESHOT",      ESC_ONESHOT },
		{"ESC_PWM",          ESC_PWM },
		{"ESC_ONESHOT42",    ESC_ONESHOT42 },
		{"ESC_DSHOT600",     ESC_DSHOT600 },
		{"ESC_DSHOT300",     ESC_DSHOT300 },
		{"ESC_DSHOT150",     ESC_DSHOT150 },
		{"ESC_MULTISHOT25",  ESC_MULTISHOT25 },
		{"ESC_MULTISHOT125", ESC_MULTISHOT125 },
//		{"ESC_MEGAVOLT",  ESC_MEGAVOLT },

		//mixer.h
		{"MIXER_X1234",     MIXER_X1234 },
		{"MIXER_X1234RY",   MIXER_X1234RY },
		{"MIXER_X1234_3D",  MIXER_X1234_3D },
		{"MIXER_X4213",     MIXER_X4213 },
		{"MIXER_X4213RY",   MIXER_X4213RY },
		{"MIXER_X4213_3D",  MIXER_X4213_3D },
		{"MIXER_CUSTOM",    MIXER_CUSTOM },

		//gyro.h
		{"LOOP_L1",   LOOP_L1   },
		{"LOOP_M1",   LOOP_M1   },
		{"LOOP_M2",   LOOP_M2   },
		{"LOOP_M4",   LOOP_M4   },
		{"LOOP_M8",   LOOP_M8   },
		{"LOOP_H16",  LOOP_H16  },
		{"LOOP_H1",   LOOP_H1   },
		{"LOOP_H2",   LOOP_H2   },
		{"LOOP_H4",   LOOP_H4   },
		{"LOOP_H8",   LOOP_H8   },
		{"LOOP_H32",  LOOP_H32  },
		{"LOOP_UH16", LOOP_UH16 },
		{"LOOP_UH1",  LOOP_UH1  },
		{"LOOP_UH2",  LOOP_UH2  },
		{"LOOP_UH4",  LOOP_UH4  },
		{"LOOP_UH8",  LOOP_UH8  },
		{"LOOP_UH32", LOOP_UH32 },

		//rx.h
		{"NO_EXPO",      NO_EXPO      },
		{"SKITZO_EXPO",  SKITZO_EXPO  },
		{"TARANIS_EXPO", TARANIS_EXPO },
		{"FAST_EXPO",    FAST_EXPO    },
		{"ACRO_PLUS",    ACRO_PLUS    },
		{"KISS_EXPO",    KISS_EXPO    },
		{"NO_EXPO",      NO_EXPO      },

		//rx.h
//		{"USING_MANUAL",           USING_MANUAL},
		{"USING_SPEK_R",           USING_SPEK_R},
		{"USING_SPEK_T",           USING_SPEK_T},
		{"USING_SBUS_R",           USING_SBUS_R},
		{"USING_SBUS_T",           USING_SBUS_T},
		{"USING_SUMD_R",           USING_SUMD_R},
		{"USING_SUMD_T",           USING_SUMD_T},
		{"USING_IBUS_R",           USING_IBUS_R},
		{"USING_IBUS_T",           USING_IBUS_T},
		{"USING_CPPM_R",           USING_CPPM_R},
		{"USING_CPPM_T",           USING_CPPM_T},

};

const config_variables_rec valueTable[] = {

		{ "mixer_type", 		typeUINT,  "mixr", &mainConfig.mixerConfig.mixerType,					0, MIXER_END, MIXER_X1234, "" },

		{ "famx", 				typeFLOAT, "mixr", &mainConfig.mixerConfig.foreAftMixerFixer,			0.9, 1.1, 1, "" },
		{ "mixer_style", 		typeUINT,  "mixr", &mainConfig.mixerConfig.mixerStyle,					0, 1, 0, "" },
		{ "esc_protocol", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escProtocol,					0, ESC_PROTOCOL_END, ESC_MULTISHOT, "" },
		{ "esc_frequency", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escUpdateFrequency,			0, 32000, 32000, "" },
		{ "idle_percent", 		typeFLOAT, "mixr", &mainConfig.mixerConfig.idlePercent,					0, 15.0, 5, "" },
		{ "idle_percent_inv",	typeFLOAT, "mixr", &mainConfig.mixerConfig.idlePercentInverted,			0, 15.0, 8, "" },

		{ "mout1", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[0],				0, 7, 0, "" },
		{ "mout2", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[1],				0, 7, 1, "" },
		{ "mout3", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[2],				0, 7, 2, "" },
		{ "mout4", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[3],				0, 7, 3, "" },
		{ "mout5", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[4],				0, 7, 4, "" },
		{ "mout6", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[5],				0, 7, 5, "" },
		{ "mout7", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[6],				0, 7, 6, "" },
		{ "mout8", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[7],				0, 7, 7, "" },

		{ "led_count",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledCount,						2, WS2812_MAX_LEDS, 16, "" },
		{ "led_red",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledRed,						0, 255, 10, "" },
		{ "led_green",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledGreen,						0, 255, 0, "" },
		{ "led_blue",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledBlue,						0, 255, 0, "" },
		{ "led_mode",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledMode,						0, 255, 0, "" },

		{ "telem_smartaudio",	typeUINT,  "telm", &mainConfig.telemConfig.telemSmartAudio,				0, TELEM_NUM-1, TELEM_OFF, "" },
		{ "telem_sport",		typeUINT,  "telm", &mainConfig.telemConfig.telemSport,					0, TELEM_NUM-1, TELEM_OFF, "" },
		{ "telem_spek",	 		typeUINT,  "telm", &mainConfig.telemConfig.telemSpek,					0, TELEM_NUM-1, TELEM_OFF, "" },
		{ "telem_msp",	 		typeUINT,  "telm", &mainConfig.telemConfig.telemMsp,					0, TELEM_NUM-1, TELEM_OFF, "" },
		{ "telem_mavlink", 		typeUINT,  "telm", &mainConfig.telemConfig.telemMav,					0, TELEM_NUM-1, TELEM_OFF, "" },
		{ "adc_current_factor", typeFLOAT, "telm", &mainConfig.telemConfig.adcCurrFactor,				0, 50.0, 0, "" },
		{ "vtx_pitmode_type",	typeUINT,  "telm", &mainConfig.telemConfig.vtxPitmodeType,				0, 1, 0, "" },

		{ "gyro_rotation", 		typeUINT,  "gyro", &mainConfig.gyroConfig.gyroRotation,					0, CW315_INV, CW0, "" },
		{ "board_calibrated", 	typeUINT,  "gyro", &mainConfig.gyroConfig.boardCalibrated,				0, 1,  0, "" },
		{ "sml_board_rot_x", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[X],		-180, 180, 0, "" },
		{ "sml_board_rot_y", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Y],		-180, 180, 0, "" },
		{ "sml_board_rot_z", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Z], 		-180, 180, 0, "" },
		{ "rf_loop_ctrl", 		typeUINT,  "gyro", &mainConfig.gyroConfig.loopCtrl, 					0, LOOP_UH32, LOOP_UH32, "" },

		{ "yaw_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kp, 						0, 500, 130.00, "" },
		{ "roll_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kp, 						0, 500, 110.00, "" },
		{ "pitch_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kp, 					0, 500, 120.00, "" },

		{ "yaw_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].ki, 						0, 3000, 900.00, "" },
		{ "roll_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].ki, 						0, 3000, 800.00, "" },
		{ "pitch_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].ki, 					0, 3000, 850.00, "" },

		{ "yaw_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kd, 						0, 3000, 1200.00, "" },
		{ "roll_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kd, 						0, 3000, 800.00, "" },
		{ "pitch_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kd, 					0, 3000, 1000.00, "" },

		{ "yaw_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[YAW].ga, 						0, 32, 4, "" },
		{ "roll_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[ROLL].ga, 						0, 32, 0, "" },
		{ "pitch_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[PITCH].ga, 					0, 32, 0, "" },

		{ "slp", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].slp, 					0, 25.0, 05.0, "" },
		{ "sli", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].sli, 					0, 25.0, 00.1, "" },
		{ "sla", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].sla, 					0, 75.0, 35.0, "" },
		{ "sld", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].sld, 					0, 0.90, 0.03, "" },

		{ "filter_mode0",		typeUINT,  "filt", &mainConfig.filterConfig[0].filterMod, 				0, 10, 0, "" },
		{ "filter_mode1",		typeUINT,  "filt", &mainConfig.filterConfig[1].filterMod, 				0, 10, 0, "" },
		{ "filter_mode2",		typeUINT,  "filt", &mainConfig.filterConfig[2].filterMod, 				0, 10, 2, "" },

		{ "yaw_quick", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.q, 				0, 100, 30.000, "" },
		{ "yaw_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.r, 				0, 200, 88.000, "" },


		{ "roll_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.q, 				0, 100, 60.000, "" },
		{ "roll_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.r, 				0, 200, 88.000, "" },


		{ "pitch_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.q, 				0, 100, 60.000, "" },
		{ "pitch_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.r, 				0, 200, 88.000, "" },


		{ "yaw_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kd.r, 					0, 100, 90.000, "" },
		{ "roll_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kd.r, 				0, 100, 90.000, "" },
		{ "pitch_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kd.r, 				0, 100, 90.000, "" },


		{ "x_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.q, 				0, 10, 2.0000, "" },
		{ "x_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.r, 				0, 10, 025.00, "" },
		{ "y_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.q, 				0, 10, 2.0000, "" },
		{ "y_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.r, 				0, 10, 025.00, "" },
		{ "z_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.q, 				0, 10, 2.0000, "" },
		{ "z_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.r, 				0, 10, 025.00, "" },

#ifdef STM32F446xx	//TODO remove target specific ifdefs
		{ "rx_protocol",		typeUINT, "rccf", &mainConfig.rcControlsConfig.rxProtcol,				0, USING_RX_END - 1, USING_SPEK_T, "" },
		{ "rx_usart",			typeUINT, "rccf", &mainConfig.rcControlsConfig.rxUsart,					0, MAX_USARTS - 1, ENUM_USART5, "" },
#else
		{ "rx_protocol", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.rxProtcol, 				0, USING_RX_END-1, USING_SPEK_T, "" },
		{ "rx_usart", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.rxUsart, 				0, MAX_USARTS-1, ENUM_USART1, "" },
#endif

		{ "arm_method", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.armMethod,				0, 1, 1, "" },
		{ "rx_inv_direction", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.rxInvertDirection,		0, 2, 0, "" },

		{ "pitch_deadband", 	typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[PITCH], 		0, 0.1, 0.004, "" },
		{ "roll_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[ROLL], 		0, 0.1, 0.004, "" },
		{ "yaw_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[YAW], 			0, 0.1, 0.005, "" },
		{ "throttle_deadband", 	typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[THROTTLE], 	0, 0.1, 0, "" },
		{ "aux1_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX1], 		0, 0.1, 0, "" },
		{ "aux2_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX2], 		0, 0.1, 0, "" },
		{ "aux3_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX3], 		0, 0.1, 0, "" },
		{ "aux4_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX4], 		0, 0.1, 0, "" },


		{ "pitch_midrc",		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[PITCH], 			0, 21480, 1024, "" },
		{ "roll_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[ROLL], 			0, 21480, 1024, "" },
		{ "yaw_midrc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[YAW], 			0, 21480, 1024, "" },
		{ "throttle_midrc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[THROTTLE],		0, 21480, 1024, "" },
		{ "aux1_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX1], 			0, 21480, 1024, "" },
		{ "aux2_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX2], 			0, 21480, 1024, "" },
		{ "aux3_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX3], 			0, 21480, 1024, "" },
		{ "aux4_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX4], 			0, 21480, 1024, "" },

		{ "pitch_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[PITCH], 			0, 21480, 0, "" },
		{ "roll_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[ROLL], 			0, 21480, 0, "" },
		{ "yaw_minrc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[YAW], 			0, 21480, 0, "" },
		{ "throttle_minrc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[THROTTLE], 		0, 21480, 0, "" },
		{ "aux1_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX1], 			0, 21480, 0, "" },
		{ "aux2_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX2], 			0, 21480, 0, "" },
		{ "aux3_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX3], 			0, 21480, 0, "" },
		{ "aux4_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX4], 			0, 21480, 0, "" },

		{ "pitch_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[PITCH], 			0, 21480, 2048, "" },
		{ "roll_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[ROLL], 			0, 21480, 2048, "" },
		{ "yaw_maxrc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[YAW], 			0, 21480, 2048, "" },
		{ "throttle_maxrc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[THROTTLE], 		0, 21480, 2048, "" },
		{ "aux1_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX1], 			0, 21480, 2048, "" },
		{ "aux2_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX2], 			0, 21480, 2048, "" },
		{ "aux3_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX3], 			0, 21480, 2048, "" },
		{ "aux4_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX4], 			0, 21480, 2048, "" },

		{ "pitch_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[PITCH], 		0, 100, 2, "" },
		{ "roll_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[ROLL], 		0, 100, 1, "" },
		{ "yaw_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[YAW], 		0, 100, 3, "" },
		{ "throttle_map", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[THROTTLE],	0, 100, 0, "" },
		{ "aux1_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX1], 		0, 100, 4, "" },
		{ "aux2_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX2], 		0, 100, 5, "" },
		{ "aux3_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX3], 		0, 100, 6, "" },
		{ "aux4_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX4], 		0, 100, 7, "" },
		{ "aux5_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX5], 		0, 100, 100, "" },
		{ "aux6_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX6], 		0, 100, 100, "" },
		{ "aux7_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX7], 		0, 100, 100, "" },
		{ "aux8_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX8], 		0, 100, 100, "" },
		{ "aux9_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX9], 		0, 100, 100, "" },
		{ "aux10_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX10], 		0, 100, 100, "" },
		{ "aux11_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX11], 		0, 100, 100, "" },
		{ "aux12_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX12], 		0, 100, 100, "" },

		{ "rc_calibrated", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.rcCalibrated,			0, 1, 0, "" },

		{ "stick_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[PITCH], 		0, EXPO_CURVE_END, ACRO_PLUS, "" },
		{ "throttle_curve", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[THROTTLE], 	0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux1_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX1], 		0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux2_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX2], 		0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux3_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX3], 		0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux4_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX4], 		0, EXPO_CURVE_END, NO_EXPO, "" },

		{ "pitch_expo", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[PITCH],		0, 100, 50, "" },
		{ "roll_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[ROLL],		0, 100, 50, "" },
		{ "yaw_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[YAW],			0, 100, 50, "" },
		{ "throttle_expo", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[THROTTLE],	0, 100, 0, "" },
		{ "aux1_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX1],		0, 100, 0, "" },
		{ "aux2_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX2],		0, 100, 0, "" },
		{ "aux3_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX3],		0, 100, 0, "" },
		{ "aux4_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[AUX4],		0, 100, 0, "" },

		{ "bind", 	            typeUINT,  "rccf", &mainConfig.rcControlsConfig.bind, 	                0, 32, 0, "" },

		{ "pitch_rate", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[PITCH],			0, 1500, 400, "" },
		{ "roll_rate", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[ROLL],			0, 1500, 400, "" },
		{ "yaw_rate", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[YAW],				0, 1500, 400, "" },

		{ "pitch_acrop", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[PITCH],		0, 300, 140, "" },
		{ "roll_acrop", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[ROLL],			0, 300, 140, "" },
		{ "yaw_acrop", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[YAW],			0, 300, 140, "" },

		{ "drunk", 				typeFLOAT, "filt", &mainConfig.filterConfig[0].gyro.p, 					0, 1, 0.01, "" },
		{ "skunk", 				typeFLOAT, "filt", &mainConfig.filterConfig[1].gyro.p, 					0, 1, 50, "" },

};


//TODO REwrite this better
char *ftoa(float x, char *floatString)
{
    int32_t value;
    char intString1[12];
    char intString2[12] = { 0, };
    char *decimalPoint = ".";
    uint8_t dpLocation;

    if (x > 0)
        x += 0.0005f;
    else
        x -= 0.0005f;

    value = (int32_t)(x * 1000.0f);

    itoa(ABS(value), intString1, 10);

    if (value >= 0)
        intString2[0] = ' ';
    else
        intString2[0] = '-';

    if (strlen(intString1) == 1) {
        intString2[1] = '0';
        intString2[2] = '0';
        intString2[3] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 2) {
        intString2[1] = '0';
        intString2[2] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 3) {
        intString2[1] = '0';
        strcat(intString2, intString1);
    } else {
        strcat(intString2, intString1);
    }

    dpLocation = strlen(intString2) - 3;

    strncpy(floatString, intString2, dpLocation);
    floatString[dpLocation] = '\0';
    strcat(floatString, decimalPoint);
    strcat(floatString, intString2 + dpLocation);

    return(floatString);
}

char *ftoa6(float x, char *floatString)
{
    int32_t value;
    char intString1[12];
    char intString2[12] = { 0, };
    char *decimalPoint = ".";
    uint8_t dpLocation;

    if (x > 0)
        x += 0.0000005f;
    else
        x -= 0.0000005f;

    value = (int32_t)(x * 1000000.0f);

    itoa(ABS(value), intString1, 10);

    if (value >= 0)
        intString2[0] = ' ';
    else
        intString2[0] = '-';

    if (strlen(intString1) == 1) {
        intString2[1] = '0';
        intString2[2] = '0';
        intString2[3] = '0';
        intString2[4] = '0';
        intString2[5] = '0';
        intString2[6] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 2) {
        intString2[1] = '0';
        intString2[2] = '0';
        intString2[3] = '0';
		intString2[4] = '0';
		intString2[5] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 3) {
        intString2[1] = '0';
        intString2[2] = '0';
        intString2[3] = '0';
        intString2[4] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 4) {
    	intString2[1] = '0';
		intString2[2] = '0';
		intString2[3] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 5) {
    	intString2[1] = '0';
		intString2[2] = '0';
        strcat(intString2, intString1);
    } else if (strlen(intString1) == 6) {
        intString2[1] = '0';
        strcat(intString2, intString1);
    } else {
        strcat(intString2, intString1);
    }

    dpLocation = strlen(intString2) - 6;

    strncpy(floatString, intString2, dpLocation);
    floatString[dpLocation] = '\0';
    strcat(floatString, decimalPoint);
    strcat(floatString, intString2 + dpLocation);

    return(floatString);
}

void ValidateConfigSettings(void)
{
	uint32_t x;

	for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
	{
		switch(valueTable[x].type)
		{
			case typeUINT:
				if ( ( *(uint32_t *)valueTable[x].ptr < (uint32_t)valueTable[x].Min ) || ( *(uint32_t *)valueTable[x].ptr > (uint32_t)valueTable[x].Max ) )
					*(uint32_t *)valueTable[x].ptr = (uint32_t)valueTable[x].Default;
				break;

			case typeINT:
				if ( ( *(int32_t *)valueTable[x].ptr < (int32_t)valueTable[x].Min ) || ( *(int32_t *)valueTable[x].ptr > (int32_t)valueTable[x].Max ) )
					*(int32_t *)valueTable[x].ptr = (int32_t)valueTable[x].Default;
				break;

			case typeFLOAT:
				if ( ( *(float *)valueTable[x].ptr < (float)valueTable[x].Min ) || ( *(float *)valueTable[x].ptr > (float)valueTable[x].Max ) )
					*(float *)valueTable[x].ptr = (float)valueTable[x].Default;
				break;

		}

	}

}

void GenerateConfig(void)
{
	uint32_t x;

	bzero(&mainConfig, sizeof(mainConfig));

	for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
	{
		switch(valueTable[x].type)
		{
		case typeUINT:
			*(uint32_t *)valueTable[x].ptr = (uint32_t)valueTable[x].Default;
			break;

		case typeINT:
			*(int32_t *)valueTable[x].ptr = (int32_t)valueTable[x].Default;
			break;

		case typeFLOAT:
			*(float *)valueTable[x].ptr = (float)valueTable[x].Default;
			break;
/*
		case typeSTRING:
			break;
*/

		}

	}


}

void SaveConfig (uint32_t addresConfigStart)
{

	uint32_t addressOffset;

	DisarmBoard();
	SKIP_GYRO=1;
	if (resetBoard) {

		//This function does a shotgun startup of the flight code.
		InitFlight();

	}

	mainConfig.version  = CONFIG_VERSION;
	mainConfig.size     = sizeof(main_config);
	mainConfig.czechsum = GetChecksum8((const uint8_t *) &mainConfig, sizeof(main_config));

	EraseFlash(addresConfigStart, addresConfigStart+sizeof(main_config));
	PrepareFlash();
	for (addressOffset = 0; addressOffset < sizeof(main_config); addressOffset += 4) {
		WriteFlash(*(uint32_t *) ((char *) &mainConfig + addressOffset), addresConfigStart+addressOffset );
	}
	FinishFlash();
	SKIP_GYRO=0;
}

static uint32_t ValidateConfig (uint32_t addresConfigStart)
{

	const main_config *temp = (main_config *) addresConfigStart; //ADDRESS_FLASH_START;
	uint8_t czechsum = 0;

	if (temp->size != sizeof(main_config))
	    return (0);

	if (CONFIG_VERSION != temp->version)
		return (0);

	czechsum = GetChecksum8((const uint8_t *) temp, sizeof(main_config));
	if (czechsum != 0)
		return (1);

	return (1);

}

void LoadConfig (uint32_t addresConfigStart)
{
	if (ValidateConfig(addresConfigStart) ) {
		memcpy(&mainConfig, (char *) addresConfigStart, sizeof(main_config));
		ValidateConfigSettings();
	} else {
		GenerateConfig();
		SaveConfig(addresConfigStart);
	}
}




//cleanup string // strip continuous spaces, first space, and non allowed characters
char *StripSpaces(char *inString)
{
	uint16_t head = 0;
	uint16_t position = 0;
	uint8_t inQuote = 0;
	uint16_t inStringLength = strlen(inString);

	for (position = 0; position < inStringLength; position++)
	{
		if (inString[position] == '"')
			inQuote = inQuote ^ 1;

		if ((inQuote) || (inString[position] != ' '))
			inString[head++] = inString[position];
	}


	inString[head] = 0;

	return (inString);
}

char *CleanupNumberString(char *inString)
{
	uint16_t head = 0;
	uint16_t position = 0;
	uint16_t inStringLength = strlen(inString);

	for (position = 0; position < inStringLength; position++)
	{
		if (inString[position] == ' ') // removes multiple spaces in a row
			continue;

		if (isdigit((unsigned char)inString[position]) || (unsigned char)inString[position] == '.' )
		{
			inString[head++] = inString[position];
		}
	}


	inString[head] = 0;

	return (inString);
}

char *CleanupString(char *inString)
{
	char last_char = ' ';
	uint16_t head = 0;
	uint16_t position = 0;
	uint16_t inStringLength = strlen(inString);

	for (position = 0; position < inStringLength; position++)
	{
		if ((last_char == ' ') && (inString[position] == ' ')) // removes multiple spaces in a row
			continue;

		if (isalnum((unsigned char)inString[position]) || (inString[position] == ' ') || (inString[position] == '=') || (inString[position] == '"') || (inString[position] == '.') || (inString[position] == '-') || (inString[position] == '_'))
		{
			inString[head++] = inString[position];
			last_char = inString[position];
		}
	}


	inString[head] = 0;

	return (inString);
}


static int32_t GetValueFromString(char *string, const string_comp_rec thisStringCompTable[], uint32_t sizeOfArray)
{
	uint32_t x;

	//compare args with strings in stringCompTable
	for (x=0;x<(sizeOfArray/sizeof(string_comp_rec));x++)
	{
		if (!strcmp(thisStringCompTable[x].valueString, string))
		{
			return(thisStringCompTable[x].valueInt);
		}
	}

	return(-1);
}

void SetValueOrString(uint32_t position, char *value)
{
	uint32_t x;
	char stringBuffer[10];

	value = CleanupNumberString(value);

	//compare args with strings in stringCompTable
	for (x=0;x<(sizeof(stringCompTable)/sizeof(string_comp_rec));x++)
	{
		if (!strcmp(stringCompTable[x].valueString, value))
		{

			//snprintf(buffer, 10, "%d", value);
			snprintf(stringBuffer, 10, "%ld", stringCompTable[x].valueInt);
			SetValue(position, stringBuffer);
			return;
		}
	}

	SetValue(position, value);
}

void SetValue(uint32_t position, char *value)
{

	switch (valueTable[position].type) {
		//TODO used something better then atoi
		case typeUINT:
		case typeINT:
			*(uint32_t *)valueTable[position].ptr = atoi(value);
			break;

		case typeFLOAT:
			*(float *)valueTable[position].ptr = atof(value);
			break;
	}

}

void SendStatusReport(char *inString)
{
	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ss %s\n", inString);
	RfCustomReplyBuffer(rf_custom_out_buffer);
}

int32_t SetVariable(char *inString)
{
	uint32_t x;
	uint32_t inStringLength;
	char *args = NULL;
	StripSpaces(inString);

	inStringLength = strlen(inString);

	for (x = 0; x < inStringLength; x++) {
		if (inString[x] == '=')
			break;
	}

	if (inStringLength > x) {
		args = inString + x + 1;
	}

	inString[x] = 0;

	for (x = 0; x < strlen(inString); x++)
		inString[x] = tolower((unsigned char)inString[x]);

	for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
	{
		if (!strcmp(valueTable[x].name, inString))
		{
			SetValueOrString(x, args);
			if ( (!strcmp(valueTable[x].group, "telm")) || (!strcmp(valueTable[x].group, "mixr")) || (!strcmp(valueTable[x].group, "gyro")) || (!strcmp(valueTable[x].group, "filt")) || (!strcmp(valueTable[x].group, "rccf"))  || (!strcmp(valueTable[x].group, "rate")) )
			{
				resetBoard=1;
			}
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me %s=%s\n", inString, args);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			return (1);
		}
	}
	return (0);
}




/**********************************************************************************************************/
void OutputVarSet(uint32_t position)
{
	char fString[20];

	switch (valueTable[position].type) {

	case typeUINT:
		sprintf(rf_custom_out_buffer, "set %s=%d\n", valueTable[position].name, (int)*(uint32_t *)valueTable[position].ptr);
		RfCustomReplyBuffer(rf_custom_out_buffer);
		break;


	case typeINT:
		sprintf(rf_custom_out_buffer, "set %s=%d\n", valueTable[position].name, (int)*(int32_t *)valueTable[position].ptr);
		RfCustomReplyBuffer(rf_custom_out_buffer);
		break;


	case typeFLOAT:
		ftoa(*(float *)valueTable[position].ptr, fString);
		StripSpaces(fString);
		sprintf(rf_custom_out_buffer, "set %s=%s\n", valueTable[position].name, fString);
		RfCustomReplyBuffer(rf_custom_out_buffer);
		break;
	}

}

void OutputVar(uint32_t position)
{
	char fString[20];

	switch (valueTable[position].type) {

	case typeUINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%d\n", valueTable[position].name, (int)*(uint32_t *)valueTable[position].ptr);
		break;


	case typeINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%d\n", valueTable[position].name, (int)*(int32_t *)valueTable[position].ptr);
		break;


	case typeFLOAT:
		ftoa(*(float *)valueTable[position].ptr, fString);
		StripSpaces(fString);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%s\n", valueTable[position].name, fString);
		break;
	}

	RfCustomReplyBuffer(rf_custom_out_buffer);
}
/**********************************************************************************************************************/


int RfCustomReply(char *rf_custom_out_buffer)
{

	bzero((rfReplyBuffer+1), (sizeof(rfReplyBuffer)-1));
	rfReplyBuffer[0]=1;
	memcpy((char *)(rfReplyBuffer+1), rf_custom_out_buffer, RF_BUFFER_SIZE);

	USBD_HID_SendReport(&hUsbDeviceFS, rfReplyBuffer, HID_EPIN_SIZE);
	return(0);

}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////COMMANDS/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//process commands here.
void ProcessCommand(char *inString)
{
	//buffer_record *buffer = &flashInfo.buffer[flashInfo.bufferNum];
	uint32_t inStringLength;
	char *args = NULL;
	char *originalString = inString;
	uint32_t x;
	static uint32_t lastTimeMore = 0;


	if (rfCustomReplyBufferPointerSent < rfCustomReplyBufferPointer)
	{
		//four second more timeout
		if (Micros() - lastTimeMore < 4000000)
		{
			SendRfCustomReplyBuffer();
			return;
		}
		else
		{
			rfCustomReplyBufferPointerSent = 0;
			rfCustomReplyBufferPointer = 0;
		}
	}

	if (originalString[1] == 77)
	{
		inString = originalString;
	}
	else
	{
		inString = CleanupString(inString);
	}

	inStringLength = strlen(inString);

	for (x = 0; x < inStringLength; x++)
	{
		if ( (inString[x] == ' ') || (inString[x] == 220) )
			break;
	}

	if (inStringLength > x)
	{
		args = inString + x + 1;
	}

	inString[x] = 0;


	args = CleanupString(args);

	inStringLength = strlen(args);

	for (x = 0; x < inStringLength; x++)
	{
		if ( (inString[x] == ' ') || (inString[x] == 220) )
		{
			args[x] = 0;
			break;
		}

	}


	for (x = 0; x < strlen(inString); x++)
		inString[x] = tolower((unsigned char)inString[x]);


	//ignore any string that starts with #
	if (inString[0] == '#')
		return;


	if (!strcmp("more", inString))
		{
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#nomore\n" );
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("polladc", inString))
		{
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Voltage: %lu, Current: %lu, mAh: %lu\n", (uint32_t)(adcVoltage*100) , (uint32_t)(adcCurrent*100), (uint32_t)(adcMAh));
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("idle", inString))
		{
			uint32_t motorToSpin = CONSTRAIN( atoi(args),0, MAX_MOTOR_NUMBER);
			DisarmBoard();
			SKIP_GYRO=1;

			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Spinning Motor %lu\n", motorToSpin );
			RfCustomReplyBuffer(rf_custom_out_buffer);

			DelayMs(10);
			IdleActuator( motorToSpin );
		}

	else if (!strcmp("fakerx", inString))
		{
			rxData[0] = 1100;
			rxData[1] = 1200;
			rxData[2] = 1300;
			rxData[3] = 1400;
			rxData[4] = 1500;
			rxData[5] = 1600;
			rxData[6] = 1700;
			rxData[7] = 1800;
			trueRcCommandF[0] = -1.00f;
			trueRcCommandF[1] = -0.75f;
			trueRcCommandF[2] = -0.25f;
			trueRcCommandF[3] =  0.00f;
			trueRcCommandF[4] =  0.25f;
			trueRcCommandF[5] =  0.50f;
			trueRcCommandF[6] =  0.75f;
			trueRcCommandF[7] =  1.00f;

			RfCustomReplyBuffer("#me RX Data has been faked\n");

		}
	else if (!strcmp("idlestop", inString))
		{
			DisarmBoard();
			ZeroActuators( 1000 );
			SKIP_GYRO=0;
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me idlestop\n" );
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("error", inString))
		{

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%li %lu %lu\n", deviceWhoAmI, errorMask, failsafeHappend);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ba:%lu\n", armingStructure.boardArmed);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "lfa:%lu\n", armingStructure.latchFirstArm);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ams:%lu\n", armingStructure.armModeSet);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "amm:%lu\n", armingStructure.armModeActive);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "rcc:%lu\n", armingStructure.rcCalibrated);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "bc:%lu\n", armingStructure.boardCalibrated);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "pm:%lu\n", armingStructure.progMode);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "tis:%lu\n", armingStructure.throttleIsSafe);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "rxt:%lu\n", armingStructure.rxTimeout);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "fh:%lu\n", armingStructure.failsafeHappend);RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "af:%lu\n", armingStructure.activeFailsafe);RfCustomReplyBuffer(rf_custom_out_buffer);

		}
	else if (!strcmp("fakeflash", inString))
		{
			flashInfo.enabled = FLASH_ENABLED;
			//flashInfo.currentWriteAddress = atoi(args);
			flashCountdownFake = 100;

			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me It's a FAAAAKE\n" );
			RfCustomReplyBuffer(rf_custom_out_buffer);

		}
	else if (!strcmp("set", inString))
		{
			if (!SetVariable(args))
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Setting Not Found:%s\n", inString);
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rxraw", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#rw %u=%u\n", (volatile unsigned int)xx+1, (volatile unsigned int)(rxDataRaw[xx]));
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rxdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#rx %u=%u\n", (volatile unsigned int)xx+1, (volatile unsigned int)(rxData[xx]));
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rcdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#rc %d=%d\n", (volatile int)xx+1, (volatile int)(trueRcCommandF[xx]*1000));
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rxrcdata", inString) || !strcmp("rcrxdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#rb %u=%u:%d\n", (volatile unsigned int)(xx+1), (volatile unsigned int)(rxData[xx]), (volatile int)(trueRcCommandF[xx]*1000));
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("telem", inString))
		{
			char pitchString[12];
			char rollString[12];
			char yawString[12];
			char ax[12];
			char ay[12];
			char az[12];
			char gx[12];
			char gy[12];
			char gz[12];
			char qx[12];
			char qy[12];
			char qz[12];
			char qw[12];

			ftoa(pitchAttitude, pitchString);
			ftoa(rollAttitude, rollString);
			ftoa(yawAttitude, yawString);
			StripSpaces(pitchString);
			StripSpaces(rollString);
			StripSpaces(yawString);

			ftoa(filteredAccData[ACCX], ax);
			ftoa(filteredAccData[ACCY], ay);
			ftoa(filteredAccData[ACCZ], az);
			StripSpaces(ax);
			StripSpaces(ay);
			StripSpaces(az);


			//X is pitch, y is roll, z is yaw
			ftoa(filteredGyroData[PITCH], gx);
			ftoa(filteredGyroData[ROLL], gy);
			ftoa(filteredGyroData[YAW], gz);
			StripSpaces(gx);
			StripSpaces(gy);
			StripSpaces(gz);

			//ftoa6(attitudeFrameQuat.w, qw);
			//ftoa6(attitudeFrameQuat.x, qx);
			//ftoa6(attitudeFrameQuat.y, qy);
			//ftoa6(attitudeFrameQuat.z, qz);
			ftoa(attitudeFrameQuat.w, qw);
			ftoa(attitudeFrameQuat.x, qx);
			ftoa(attitudeFrameQuat.y, qy);
			ftoa(attitudeFrameQuat.z, qz);
			StripSpaces(qw);
			StripSpaces(qx);
			StripSpaces(qy);
			StripSpaces(qz);

			//todo: make a way to combine strings
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm pitch=%s\n#tm roll=%s\n#tm heading=%s\n", pitchString,rollString,yawString);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm ax=%s\n#tm ay=%s\n#tm az=%s\n", ax,ay,az);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm gx=%s\n#tm gy=%s\n#tm gz=%s\n", gx,gy,gz);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm qx=%s\n", qx);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm qy=%s\n", qy);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm qz=%s\n", qz);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm qw=%s\n", qw);
			RfCustomReplyBuffer(rf_custom_out_buffer);

		}
	else if (!strncmp("sbus_", inString, 4))
		{
			uint32_t protocol;
			uint32_t usart;

			switch(inString[5])
			{
				case 't':
					protocol = USING_SBUS_T;
					break;
				case 'r':
					protocol = USING_SBUS_R;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}
			switch(inString[6])
			{
				case '1':
					usart = ENUM_USART1;
					break;
				case '2':
					usart = ENUM_USART2;
					break;
				case '3':
					usart = ENUM_USART3;
					break;
				case '4':
					usart = ENUM_USART4;
					break;
				case '5':
					usart = ENUM_USART5;
					break;
				case '6':
					usart = ENUM_USART6;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me SBUS Defaults\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();
		}
	else if (!strncmp("sumd_", inString, 4))
		{

			uint32_t protocol;
			uint32_t usart;

			switch(inString[5])
			{
				case 't':
					protocol = USING_SUMD_T;
					break;
				case 'r':
					protocol = USING_SUMD_R;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}
			switch(inString[6])
			{
				case '1':
					usart = ENUM_USART1;
					break;
				case '2':
					usart = ENUM_USART2;
					break;
				case '3':
					usart = ENUM_USART3;
					break;
				case '4':
					usart = ENUM_USART4;
					break;
				case '5':
					usart = ENUM_USART5;
					break;
				case '6':
					usart = ENUM_USART6;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me SUMD Defaults\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();

		}
	else if (!strncmp("ibus_", inString, 4))
		{

			uint32_t protocol;
			uint32_t usart;

			switch(inString[5])
			{
				case 't':
					protocol = USING_IBUS_T;
					break;
				case 'r':
					protocol = USING_IBUS_R;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}
			switch(inString[6])
			{
				case '1':
					usart = ENUM_USART1;
					break;
				case '2':
					usart = ENUM_USART2;
					break;
				case '3':
					usart = ENUM_USART3;
					break;
				case '4':
					usart = ENUM_USART4;
					break;
				case '5':
					usart = ENUM_USART5;
					break;
				case '6':
					usart = ENUM_USART6;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me IBUS Defaults\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();
		}
	else if (!strncmp("cppm_", inString, 4))
		{
			uint32_t protocol;
			uint32_t usart;

			switch(inString[5])
			{
				case 't':
					protocol = USING_CPPM_T;
					break;
				case 'r':
					protocol = USING_CPPM_R;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}
			switch(inString[6])
			{
				case '1':
					usart = ENUM_USART1;
					break;
				case '2':
					usart = ENUM_USART2;
					break;
				case '3':
					usart = ENUM_USART3;
					break;
				case '4':
					usart = ENUM_USART4;
					break;
				case '5':
					usart = ENUM_USART5;
					break;
				case '6':
					usart = ENUM_USART6;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			RfCustomReplyBuffer("#me CPPM Defaults\n");

			SaveAndSend();
		}
		else if (!strcmp("nytfluffyrates", inString))
		{

			mainConfig.rcControlsConfig.useCurve[PITCH]    = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[ROLL]     = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[YAW]      = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[THROTTLE] = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX1]     = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX2]     = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX3]     = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX4]     = NO_EXPO;

			mainConfig.rcControlsConfig.rates[PITCH]       = 300;
			mainConfig.rcControlsConfig.rates[ROLL]        = 300;
			mainConfig.rcControlsConfig.rates[YAW]         = 150;

			mainConfig.rcControlsConfig.curveExpo[PITCH]   = 50;
			mainConfig.rcControlsConfig.curveExpo[ROLL]    = 50;
			mainConfig.rcControlsConfig.curveExpo[YAW]     = 20;

			mainConfig.rcControlsConfig.acroPlus[PITCH]    = 58;
			mainConfig.rcControlsConfig.acroPlus[ROLL]     = 58;
			mainConfig.rcControlsConfig.acroPlus[YAW]      = 40;

			resetBoard = 1;

			RfCustomReplyBuffer("#me Nyfluffy mode engaged\n");

			SaveAndSend();

		}
		else if (!strcmp("nytfluffyall", inString))
		{

			SetRxDefaults(USING_SBUS_T, ENUM_USART1);
			SetMode(M_ARMED, 4, 50, 100);
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			mainConfig.rcControlsConfig.useCurve[PITCH]    = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[ROLL]     = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[YAW]      = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[THROTTLE] = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX1]     = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX2]     = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX3]     = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX4]     = NO_EXPO;

			mainConfig.rcControlsConfig.rates[PITCH]       = 300;
			mainConfig.rcControlsConfig.rates[ROLL]        = 300;
			mainConfig.rcControlsConfig.rates[YAW]         = 150;

			mainConfig.rcControlsConfig.curveExpo[PITCH]   = 50;
			mainConfig.rcControlsConfig.curveExpo[ROLL]    = 50;
			mainConfig.rcControlsConfig.curveExpo[YAW]     = 20;

			mainConfig.rcControlsConfig.acroPlus[PITCH]    = 58;
			mainConfig.rcControlsConfig.acroPlus[ROLL]     = 58;
			mainConfig.rcControlsConfig.acroPlus[YAW]      = 40;

			resetBoard = 1;

			RfCustomReplyBuffer("#me Nyfluffy mode engaged\n");

			SaveAndSend();

		}
		else if (!strcmp("willard", inString))
		{

			SetRxDefaults(USING_SBUS_T, ENUM_USART1);
			SetMode(M_ARMED, 4, 50, 100);
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			mainConfig.rcControlsConfig.midRc[YAW]           = 999;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 971;
			mainConfig.rcControlsConfig.midRc[PITCH]         = 982;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 981;

			mainConfig.rcControlsConfig.minRc[YAW]           = 195;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 172;
			mainConfig.rcControlsConfig.minRc[PITCH]         = 172;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 176;

			mainConfig.rcControlsConfig.maxRc[YAW]           = 1811;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 1811;
			mainConfig.rcControlsConfig.maxRc[PITCH]         = 1792;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 1811;

			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 0;
			mainConfig.rcControlsConfig.channelMap[PITCH]    = 1;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 2;

			mainConfig.rcControlsConfig.useCurve[PITCH]      = KISS_EXPO;
			mainConfig.rcControlsConfig.useCurve[ROLL]       = KISS_EXPO;
			mainConfig.rcControlsConfig.useCurve[YAW]        = KISS_EXPO;
			mainConfig.rcControlsConfig.useCurve[THROTTLE]   = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX1]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX2]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX3]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX4]       = NO_EXPO;

			mainConfig.rcControlsConfig.rates[PITCH]         = 0.75;
			mainConfig.rcControlsConfig.rates[ROLL]          = 0.75;
			mainConfig.rcControlsConfig.rates[YAW]           = 0.75;

			mainConfig.rcControlsConfig.curveExpo[PITCH]     = 0.2;
			mainConfig.rcControlsConfig.curveExpo[ROLL]      = 0.2;
			mainConfig.rcControlsConfig.curveExpo[YAW]       = 0.13;

			mainConfig.rcControlsConfig.acroPlus[PITCH]      = 1.28;
			mainConfig.rcControlsConfig.acroPlus[ROLL]       = 1.28;
			mainConfig.rcControlsConfig.acroPlus[YAW]        = 1.15;

			mainConfig.mixerConfig.mixerType                 = 0;

			mainConfig.gyroConfig.gyroRotation               = CW0;
			mainConfig.gyroConfig.boardCalibrated            = 1;

			resetBoard = 1;

			RfCustomReplyBuffer("#me Willard mode engaged\n");

			SaveAndSend();

		}
		else if (!strcmp("rs2k", inString))
		{

			SetRxDefaults(USING_SPEK_T, ENUM_USART3);
			SetMode(M_ARMED, 4, 50, 100);
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			mainConfig.mixerConfig.mixerType                 = 1;

			mainConfig.rcControlsConfig.useCurve[PITCH]      = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[ROLL]       = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[YAW]        = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[THROTTLE]   = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX1]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX2]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX3]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX4]       = NO_EXPO;

			mainConfig.rcControlsConfig.rates[PITCH]         = 400;
			mainConfig.rcControlsConfig.rates[ROLL]          = 400;
			mainConfig.rcControlsConfig.rates[YAW]           = 400;

			mainConfig.rcControlsConfig.curveExpo[PITCH]     = 50;
			mainConfig.rcControlsConfig.curveExpo[ROLL]      = 50;
			mainConfig.rcControlsConfig.curveExpo[YAW]       = 50;

			mainConfig.rcControlsConfig.acroPlus[PITCH]      = 140;
			mainConfig.rcControlsConfig.acroPlus[ROLL]       = 140;
			mainConfig.rcControlsConfig.acroPlus[YAW]        = 140;

			mainConfig.gyroConfig.gyroRotation               = CW0;
			mainConfig.gyroConfig.boardCalibrated            = 1;

			resetBoard = 1;

			RfCustomReplyBuffer("#me RS2K mode engaged\n");

			SaveAndSend();

		}
		else if (!strcmp("braindrainall", inString))
		{

			mainConfig.mixerConfig.mixerType                 = MIXER_X1234RY;
			mainConfig.rcControlsConfig.midRc[PITCH]         = 1024;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 1024;
			mainConfig.rcControlsConfig.midRc[YAW]           = 1024;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1024;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 1024;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 22;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 22;
			mainConfig.rcControlsConfig.minRc[YAW]           = 22;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 22;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 342;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2025;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2025;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2025;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2025;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1706;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 100;

			mainConfig.rcControlsConfig.rcCalibrated         = 1;

			mainConfig.rcControlsConfig.rxUsart              = ENUM_USART3;
			mainConfig.rcControlsConfig.rxProtcol            = USING_SPEK_T; //this is used by serial.c

			SetMode(M_ARMED, 4, 50, 100);

			mainConfig.pidConfig[YAW].kp     = 135.00;
			mainConfig.pidConfig[ROLL].kp    = 115.00;
			mainConfig.pidConfig[PITCH].kp   = 120.00;

			mainConfig.pidConfig[YAW].ki     = 1000.00;
			mainConfig.pidConfig[ROLL].ki    = 900.00;
			mainConfig.pidConfig[PITCH].ki   = 1000.00;

			mainConfig.pidConfig[YAW].kd     = 1200.00;
			mainConfig.pidConfig[ROLL].kd    = 1000.00;
			mainConfig.pidConfig[PITCH].kd   = 1200.00;

			mainConfig.rcControlsConfig.useCurve[PITCH]      = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[ROLL]       = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[YAW]        = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[THROTTLE]   = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX1]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX2]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX3]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX4]       = NO_EXPO;

			mainConfig.rcControlsConfig.rates[PITCH]         = 300;
			mainConfig.rcControlsConfig.rates[ROLL]          = 350;
			mainConfig.rcControlsConfig.rates[YAW]           = 350;

			mainConfig.rcControlsConfig.curveExpo[PITCH]     = 60;
			mainConfig.rcControlsConfig.curveExpo[ROLL]      = 60;
			mainConfig.rcControlsConfig.curveExpo[YAW]       = 25;

			mainConfig.rcControlsConfig.acroPlus[PITCH]      = 0;
			mainConfig.rcControlsConfig.acroPlus[ROLL]       = 0;
			mainConfig.rcControlsConfig.acroPlus[YAW]        = 0;

			mainConfig.gyroConfig.gyroRotation               = CW0;
			mainConfig.gyroConfig.boardCalibrated            = 1;

			resetBoard = 1;

			RfCustomReplyBuffer("#me Braindrain mode engaged\n");

			SaveAndSend();

		}
		else if (!strcmp("braindrainrates", inString))
		{
			mainConfig.rcControlsConfig.useCurve[PITCH]      = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[ROLL]       = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[YAW]        = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[THROTTLE]   = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX1]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX2]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX3]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX4]       = NO_EXPO;

			mainConfig.rcControlsConfig.rates[PITCH]         = 300;
			mainConfig.rcControlsConfig.rates[ROLL]          = 320;
			mainConfig.rcControlsConfig.rates[YAW]           = 280;

			mainConfig.rcControlsConfig.curveExpo[PITCH]     = 60;
			mainConfig.rcControlsConfig.curveExpo[ROLL]      = 60;
			mainConfig.rcControlsConfig.curveExpo[YAW]       = 25;

			mainConfig.rcControlsConfig.acroPlus[PITCH]      = 0;
			mainConfig.rcControlsConfig.acroPlus[ROLL]       = 0;
			mainConfig.rcControlsConfig.acroPlus[YAW]        = 0;

			resetBoard = 1;

			RfCustomReplyBuffer("#me Braindrain mode engaged\n");

			SaveAndSend();

		}
		else if (!strcmp("420batman", inString))
		{

			//filter defaults
			mainConfig.pidConfig[YAW].ga                     = 4;
			mainConfig.pidConfig[ROLL].ga                    = 0;
			mainConfig.pidConfig[PITCH].ga                   = 0;

			mainConfig.filterConfig[YAW].gyro.r              = 88.00;
			mainConfig.filterConfig[ROLL].gyro.r             = 88.00;
			mainConfig.filterConfig[PITCH].gyro.r            = 88.00;

			mainConfig.filterConfig[YAW].gyro.q              = 30.000;
			mainConfig.filterConfig[ROLL].gyro.q             = 60.000;
			mainConfig.filterConfig[PITCH].gyro.q            = 60.000;

			mainConfig.filterConfig[0].filterMod             = 0;
			mainConfig.filterConfig[1].filterMod             = 0;
			mainConfig.filterConfig[2].filterMod             = 2;

			mainConfig.mixerConfig.mixerType                 = MIXER_X1234RY;
			mainConfig.rcControlsConfig.midRc[PITCH]         = 1024;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 1024;
			mainConfig.rcControlsConfig.midRc[YAW]           = 1024;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1024;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 1024;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 1024;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 22;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 22;
			mainConfig.rcControlsConfig.minRc[YAW]           = 22;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 22;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 342;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2025;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2025;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2025;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2025;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1706;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 7;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 100;

			mainConfig.rcControlsConfig.rcCalibrated         = 1;

			mainConfig.rcControlsConfig.rxUsart              = ENUM_USART3;
			mainConfig.rcControlsConfig.rxProtcol            = USING_SPEK_T; //this is used by serial.c

			SetMode(M_ARMED, 4, 50, 100);

			mainConfig.pidConfig[YAW].kp     = 130.00;
			mainConfig.pidConfig[ROLL].kp    = 110.00;
			mainConfig.pidConfig[PITCH].kp   = 120.00;

			mainConfig.pidConfig[YAW].ki     = 700.00;
			mainConfig.pidConfig[ROLL].ki    = 650.00;
			mainConfig.pidConfig[PITCH].ki   = 650.00;

			mainConfig.pidConfig[YAW].kd     = 1200.00;
			mainConfig.pidConfig[ROLL].kd    = 0800.00;
			mainConfig.pidConfig[PITCH].kd   = 1000.00;

			mainConfig.rcControlsConfig.useCurve[PITCH]      = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[ROLL]       = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[YAW]        = ACRO_PLUS;
			mainConfig.rcControlsConfig.useCurve[THROTTLE]   = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX1]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX2]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX3]       = NO_EXPO;
			mainConfig.rcControlsConfig.useCurve[AUX4]       = NO_EXPO;

			mainConfig.rcControlsConfig.rates[PITCH]         = 340;
			mainConfig.rcControlsConfig.rates[ROLL]          = 340;
			mainConfig.rcControlsConfig.rates[YAW]           = 340;

			mainConfig.rcControlsConfig.curveExpo[PITCH]     = 50;
			mainConfig.rcControlsConfig.curveExpo[ROLL]      = 50;
			mainConfig.rcControlsConfig.curveExpo[YAW]       = 50;

			mainConfig.rcControlsConfig.acroPlus[PITCH]      = 140;
			mainConfig.rcControlsConfig.acroPlus[ROLL]       = 140;
			mainConfig.rcControlsConfig.acroPlus[YAW]        = 140;

			mainConfig.gyroConfig.gyroRotation               = CW0;
			mainConfig.gyroConfig.boardCalibrated            = 1;

			resetBoard = 1;

			RfCustomReplyBuffer("#me You GILLED it\n");

			SaveAndSend();

		}
	else if (!strcmp("pidsdefault", inString))
		{

			mainConfig.gyroConfig.loopCtrl   = LOOP_UH32;
			mainConfig.pidConfig[YAW].kp     = 130.00;
			mainConfig.pidConfig[ROLL].kp    = 110.00;
			mainConfig.pidConfig[PITCH].kp   = 120.00;

			mainConfig.pidConfig[YAW].ki     = 900.00;
			mainConfig.pidConfig[ROLL].ki    = 800.00;
			mainConfig.pidConfig[PITCH].ki   = 850.00;

			mainConfig.pidConfig[YAW].kd     = 1200.00;
			mainConfig.pidConfig[ROLL].kd    = 0800.00;
			mainConfig.pidConfig[PITCH].kd   = 1000.00;

			mainConfig.pidConfig[YAW].ga     = 4;
			mainConfig.pidConfig[ROLL].ga    = 0;
			mainConfig.pidConfig[PITCH].ga   = 0;

			mainConfig.filterConfig[YAW].gyro.r   = 88.00;
			mainConfig.filterConfig[ROLL].gyro.r  = 88.00;
			mainConfig.filterConfig[PITCH].gyro.r = 88.00;

			mainConfig.filterConfig[YAW].gyro.q   = 30.000;
			mainConfig.filterConfig[ROLL].gyro.q  = 60.000;
			mainConfig.filterConfig[PITCH].gyro.q = 60.000;

			mainConfig.filterConfig[0].filterMod  = 0;
			mainConfig.filterConfig[1].filterMod  = 0;
			mainConfig.filterConfig[2].filterMod  = 2;

			mainConfig.filterConfig[YAW].kd.r     = 90.0;
			mainConfig.filterConfig[ROLL].kd.r    = 90.0;
			mainConfig.filterConfig[PITCH].kd.r   = 90.0;

			resetBoard = 1;

			RfCustomReplyBuffer("#me Default PIDs\n");

			SaveAndSend();

		}
	else if (!strcmp("pidsrs2k", inString))
		{

			mainConfig.gyroConfig.loopCtrl   = LOOP_UH32;
			mainConfig.pidConfig[YAW].kp     = 130.00;
			mainConfig.pidConfig[ROLL].kp    = 110.00;
			mainConfig.pidConfig[PITCH].kp   = 120.00;

			mainConfig.pidConfig[YAW].ki     = 700.00;
			mainConfig.pidConfig[ROLL].ki    = 600.00;
			mainConfig.pidConfig[PITCH].ki   = 650.00;

			mainConfig.pidConfig[YAW].kd     = 1200.00;
			mainConfig.pidConfig[ROLL].kd    = 800.00;
			mainConfig.pidConfig[PITCH].kd   = 1000.00;

			mainConfig.pidConfig[YAW].ga     = 4;
			mainConfig.pidConfig[ROLL].ga    = 0;
			mainConfig.pidConfig[PITCH].ga   = 0;

			mainConfig.filterConfig[YAW].gyro.r   = 88.00;
			mainConfig.filterConfig[ROLL].gyro.r  = 88.00;
			mainConfig.filterConfig[PITCH].gyro.r = 88.00;

			mainConfig.filterConfig[YAW].gyro.q   = 30.000;
			mainConfig.filterConfig[ROLL].gyro.q  = 60.000;
			mainConfig.filterConfig[PITCH].gyro.q = 60.000;

			mainConfig.filterConfig[0].filterMod  = 0;
			mainConfig.filterConfig[1].filterMod  = 0;
			mainConfig.filterConfig[2].filterMod  = 2;

			mainConfig.filterConfig[YAW].kd.r     = 90.0;
			mainConfig.filterConfig[ROLL].kd.r    = 90.0;
			mainConfig.filterConfig[PITCH].kd.r   = 90.0;

			resetBoard = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me RS2K PIDs\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();
		}
	else if (!strncmp("spek_", inString, 4))
		{
			uint32_t protocol;
			uint32_t usart;

			switch(inString[5])
			{
				case 't':
					protocol = USING_SPEK_T;
					break;
				case 'r':
					protocol = USING_SPEK_R;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}
			switch(inString[6])
			{
				case '1':
					usart = ENUM_USART1;
					break;
				case '2':
					usart = ENUM_USART2;
					break;
				case '3':
					usart = ENUM_USART3;
					break;
				case '4':
					usart = ENUM_USART4;
					break;
				case '5':
					usart = ENUM_USART5;
					break;
				case '6':
					usart = ENUM_USART6;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me SPEK Defaults\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();
		}
	else if (!strncmp("dsm2_", inString, 4))
		{
			uint32_t protocol;
			uint32_t usart;

			switch(inString[5])
			{
				case 't':
					protocol = USING_DSM2_T;
					break;
				case 'r':
					protocol = USING_DSM2_R;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}
			switch(inString[6])
			{
				case '1':
					usart = ENUM_USART1;
					break;
				case '2':
					usart = ENUM_USART2;
					break;
				case '3':
					usart = ENUM_USART3;
					break;
				case '4':
					usart = ENUM_USART4;
					break;
				case '5':
					usart = ENUM_USART5;
					break;
				case '6':
					usart = ENUM_USART6;
					break;
				default:
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me DSM2 Defaults\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();
		}
	else if (!strcmp("vtxinfo", inString))
		{

			InitSmartAudio();
			DeInitSmartAudio();

			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxDevice: %lu\n",      vtxRecord.vtxDevice);      RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBand: %lu\n",        vtxRecord.vtxBand);        RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxChannel: %lu\n",     vtxRecord.vtxChannel);     RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBandChannel: %lu\n", vtxRecord.vtxBandChannel); RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPower: %lu\n",       vtxRecord.vtxPower);       RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPit: %lu\n",         vtxRecord.vtxPit);         RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxRegion: %lu\n",      vtxRecord.vtxRegion);      RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxFrequency: %lu\n",   vtxRecord.vtxFrequency);   RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("vtxon", inString))
		{

			InitSmartAudio();

			if (VtxTurnOn())
			{
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxDevice: %lu\n",      vtxRecord.vtxDevice);      RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBand: %lu\n",        vtxRecord.vtxBand);        RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxChannel: %lu\n",     vtxRecord.vtxChannel);     RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBandChannel: %lu\n", vtxRecord.vtxBandChannel); RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPower: %lu\n",       vtxRecord.vtxPower);       RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPit: %lu\n",         vtxRecord.vtxPit);         RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxRegion: %lu\n",      vtxRecord.vtxRegion);      RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxFrequency: %lu\n",   vtxRecord.vtxFrequency);   RfCustomReplyBuffer(rf_custom_out_buffer);
			}
			else
			{
				RfCustomReplyBuffer("#me Error turning on VTX\n");
			}

			DeInitSmartAudio();

		}
	else if (!strcmp("vtxpit", inString))
		{

			InitSmartAudio();

			if (VtxTurnPit())
			{
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxDevice: %lu\n",      vtxRecord.vtxDevice);      RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBand: %lu\n",        vtxRecord.vtxBand);        RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxChannel: %lu\n",     vtxRecord.vtxChannel);     RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBandChannel: %lu\n", vtxRecord.vtxBandChannel); RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPower: %lu\n",       vtxRecord.vtxPower);       RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPit: %lu\n",         vtxRecord.vtxPit);         RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxRegion: %lu\n",      vtxRecord.vtxRegion);      RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxFrequency: %lu\n",   vtxRecord.vtxFrequency);   RfCustomReplyBuffer(rf_custom_out_buffer);
			}
			else
			{
				RfCustomReplyBuffer("#me Error putting VTX into pit mode\n");
			}

			DeInitSmartAudio();

		}
	else if (!strcmp("vtxbandchannel", inString))
		{

			InitSmartAudio();

			if (VtxBandChannel( GetValueFromString(args, vtxStringCompTable, sizeof(vtxStringCompTable)) ))
			{
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxDevice: %lu\n",      vtxRecord.vtxDevice);      RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBand: %lu\n",        vtxRecord.vtxBand);        RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxChannel: %lu\n",     vtxRecord.vtxChannel);     RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxBandChannel: %lu\n", vtxRecord.vtxBandChannel); RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPower: %lu\n",       vtxRecord.vtxPower);       RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxPit: %lu\n",         vtxRecord.vtxPit);         RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxRegion: %lu\n",      vtxRecord.vtxRegion);      RfCustomReplyBuffer(rf_custom_out_buffer);
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me vtx.vtxFrequency: %lu\n",   vtxRecord.vtxFrequency);   RfCustomReplyBuffer(rf_custom_out_buffer);
			}
			else
			{
				RfCustomReplyBuffer("#me Error changing VTX channel\n");
			}

			DeInitSmartAudio();

		}
	else if (!strcmp("serial", inString))
		{
			DelayMs(100);
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#serial: %lu%lu%lu\n", STM32_UUID[0], STM32_UUID[1], STM32_UUID[2] );
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("dump", inString))
		{
			uint32_t argsOutputted = 0;

			if ( (!strcmp("", args)) || (!strcmp("all", args)) )
			{

				RfCustomReplyBuffer(FULL_VERSION_STRING);
				//snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s", FULL_VERSION_STRING);
				//RfCustomReply(rf_custom_out_buffer);

				DlflStatusDump();
				PrintModes();

				for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
				{
					OutputVarSet(x);
					argsOutputted++;
				}

			}
			else
			{

				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s\n", FULL_VERSION_STRING);
				RfCustomReplyBuffer(rf_custom_out_buffer);
				for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
				{
					if (!strcmp(valueTable[x].group, args))
					{
						OutputVarSet(x);
						argsOutputted++;
					}
				}

			}

			if (argsOutputted == 0)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me No Arguments Found For:%s\n", args);
				RfCustomReplyBuffer(rf_custom_out_buffer);

			}
			//RfCustomReplyBuffer("#RFEND");
			//snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#RFEND");
			//RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("eraseallflash", inString))
		{

			if (flashInfo.enabled)
			{

				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Erasing Flash\n");
				RfCustomReplyBuffer(rf_custom_out_buffer);

				if (MassEraseDataFlash(1))
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Complete\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
				}
				else
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Failed\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);
				}

			}
			else
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Chip Not Detected\n");
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}

		}
	else if (!strcmp("eraseflash", inString))
		{
			if (flashInfo.enabled)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Erasing Flash\n");
				RfCustomReplyBuffer(rf_custom_out_buffer);

				if (((float)(flashInfo.currentWriteAddress) / (float)flashInfo.totalSize) > 0.85) {
					if (MassEraseDataFlash(1))
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Complete\n");
						RfCustomReplyBuffer(rf_custom_out_buffer);
					}
					else
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Failed\n");
						RfCustomReplyBuffer(rf_custom_out_buffer);
					}
				} else {
					if (MassEraseDataFlashByPage(1))
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Complete\n");
						RfCustomReplyBuffer(rf_custom_out_buffer);
					}
				}

			}
			else
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Chip Not Detected\n");
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("dlflstatusdump", inString))
		{
			DlflStatusDump();
		}
	else if (!strcmp("quadwall", inString))
		{
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " \\ /\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " / \\\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " \\ /\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " / \\\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " \\ /\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " / \\\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " \\ /\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, " / \\\n");
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "x   x\n");
		}
	else if (!strcmp("readflash", inString))
		{

			if (flashInfo.enabled) {

				args = StripSpaces(args);

				if ( M25p16ReadPage( atoi(args), flashInfo.buffer[0].txBuffer, flashInfo.buffer[0].rxBuffer) )
				{

					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					for (uint32_t x=0;x<RF_BUFFER_SIZE;x++)
					{
						rf_custom_out_buffer[x] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
					}
					RfCustomReply(rf_custom_out_buffer);
					return;

				}
				else
				{

					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Read Failed\n");
					RfCustomReplyBuffer(rf_custom_out_buffer);

				}

			}
			else
			{

				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Chip Not Detected\n");
				RfCustomReplyBuffer(rf_custom_out_buffer);

			}

		}
	else if (!strcmp("version", inString))
		{
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s\n", FULL_VERSION_STRING);
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("wiz", inString))
		{
			args = StripSpaces(args);
			SetupWizard(args);
		}
	else if (!strcmp("modes", inString))
		{
			args = StripSpaces(args);
			SetupModes(args);
		}
	else if (!strcmp("save", inString))
		{
			SaveAndSend();
		}
	else if (!strcmp("reboot", inString) || !strcmp("reset", inString))
		{
			RfCustomReply("#me Rebooting\n");
			SystemReset();
			return;
		}
	else if (!strcmp("resetdfu", inString)  || !strcmp("rebootdfu", inString))
		{
			RfCustomReply("#me Rebooting Into DFU\n");
			SystemResetToDfuBootloader();
			return;
		}
	else if (!strcmp("resetconfig", inString))
		{

			resetBoard = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Resetting Config\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);
			GenerateConfig();

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Config Reset\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);
			SaveAndSend();

		}
	else if (!strcmp("binds", inString))
		{
			sendSpektrumBind();
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Binding Serial\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if ( (!strcmp("bind9", inString)) || (!strcmp("bind", inString)) )
		{
			mainConfig.rcControlsConfig.bind = 9;
			RtcWriteBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			RtcWriteBackupRegister(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			SaveAndSend();
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Binding 9\n");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("bind5", inString))
		{
			mainConfig.rcControlsConfig.bind = 5;
			RtcWriteBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			RtcWriteBackupRegister(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			SaveAndSend();
			RfCustomReplyBuffer("#me Binding 5\n");
		}
	else if (!strcmp("bind3", inString))
		{
			mainConfig.rcControlsConfig.bind = 3;
			RtcWriteBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			RtcWriteBackupRegister(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			SaveAndSend();
			RfCustomReplyBuffer("#me Binding 3\n");
		}
	else if (!strcmp("rebootrfbl", inString) || !strcmp("resetrfbl", inString))
		{
			RtcWriteBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RFBL_COMMAND);
			RfCustomReply("#me Rebooting Into RFBL\n");
			DelayMs(100);
			SystemReset();
			return;
		}
	else if (!strcmp("rebootrecovery", inString) || !strcmp("resetrecovery", inString))
		{
			RtcWriteBackupRegister(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RECOVERY_COMMAND);
			RfCustomReply("#me Rebooting Into Recovery\n");
			DelayMs(100);
			SystemReset();
			return;
		}
	else if (!strcmp("1wire", inString))
		{
			args = StripSpaces(args);
			OneWire(args);
		}
	else
		{
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ms Unknown Command:%s\n", inString);
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}

	SendRfCustomReplyBuffer();
	lastTimeMore=Micros();

}

void SaveAndSend(void)
{
	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ms Saving\n");
	RfCustomReplyBuffer(rf_custom_out_buffer);
	SaveConfig(ADDRESS_CONFIG_START);
	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ms Save Complete\n");
	RfCustomReplyBuffer(rf_custom_out_buffer);
}

void DlflStatusDump(void)
{
	sprintf(rf_custom_out_buffer, "#fl size=%u\n#fl total=%u\n", (unsigned int)(flashInfo.currentWriteAddress), (unsigned int)(flashInfo.totalSize));
	RfCustomReplyBuffer(rf_custom_out_buffer);
}


int RfCustomReplyBuffer(char *rfCustomSendBufferAdder)
{

	uint32_t stringLength = strlen(rfCustomSendBufferAdder);

	if ( (stringLength+1+rfCustomReplyBufferPointer) <= LARGE_RF_BUFFER_SIZE)
	{
		memcpy(rfCustomSendBuffer+rfCustomReplyBufferPointer, rfCustomSendBufferAdder, stringLength);
		rfCustomReplyBufferPointer += stringLength;
		rfCustomSendBuffer[rfCustomReplyBufferPointer] = 0;
	}

	//add rfCustomSendBufferAdder to rfCustomSendBuffer and add a \n to the end
	//snprintf(rfCustomSendBuffer+rfCustomReplyBufferPointer, LARGE_RF_BUFFER_SIZE-rfCustomReplyBufferPointer, "%s", rfCustomSendBufferAdder);

//	rfCustomReplyBufferPointer += strlen(rfCustomSendBufferAdder) + 1; //adding a \n

	return(0);

}

int SendRfCustomReplyBuffer(void)
{

	if(rfCustomReplyBufferPointer > 0)
	{
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE+1, "%s", rfCustomSendBuffer+rfCustomReplyBufferPointerSent);
		RfCustomReply(rf_custom_out_buffer);
		rfCustomReplyBufferPointerSent +=RF_BUFFER_SIZE;
	}
	else
	{
		rfCustomReplyBufferPointerSent=0;
		rfCustomReplyBufferPointer    =0;
	}

	if (rfCustomReplyBufferPointerSent >= rfCustomReplyBufferPointer)
	{
		rfCustomReplyBufferPointerSent=0;
		rfCustomReplyBufferPointer    =0;
	}

	return(0);

}
