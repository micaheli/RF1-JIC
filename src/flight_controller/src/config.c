#include "includes.h"


// use variable record but instead of storing address of variable, store offset based on address of field, that way it works with the record loaded from file

uint32_t rfCustomReplyBufferPointer = 0;
uint32_t rfCustomReplyBufferPointerSent = 0;

#define LARGE_RF_BUFFER_SIZE 4096
main_config mainConfig;
volatile uint32_t disableSaving=0;
uint32_t sendReturn = 0;
uint32_t resetBoard = 0;
char rf_custom_out_buffer[RF_BUFFER_SIZE];
char rfCustomSendBufferAdder[RF_BUFFER_SIZE];
char rfCustomSendBuffer[LARGE_RF_BUFFER_SIZE];

static uint32_t ValidateConfig (uint32_t addresConfigStart);
static void     SetValueOrString(uint32_t position, char *value);
static void     SetValue(uint32_t position, char *value);
static void     DlflStatusDump(void);

static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


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
		{ "esc_protocol", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escProtcol,					0, ESC_PROTOCOL_END, ESC_MULTISHOT, "" },
		{ "esc_frequency", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escUpdateFrequency,			0, 32000, 32000, "" },
		{ "idle_percent", 		typeFLOAT, "mixr", &mainConfig.mixerConfig.idlePercent,					0, 15.0, 5, "" },
		{ "idle_percent_inv",	typeFLOAT, "mixr", &mainConfig.mixerConfig.idlePercentInverted,			0, 15.0, 6, "" },

		{ "mout1", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[0],				0, 7, 0, "" },
		{ "mout2", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[1],				0, 7, 1, "" },
		{ "mout3", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[2],				0, 7, 2, "" },
		{ "mout4", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[3],				0, 7, 3, "" },
		{ "mout5", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[4],				0, 7, 4, "" },
		{ "mout6", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[5],				0, 7, 5, "" },
		{ "mout7", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[6],				0, 7, 6, "" },
		{ "mout8", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[7],				0, 7, 7, "" },

		{ "led_count",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledCount,						0, WS2812_MAX_LEDS, 16, "" },
		{ "led_red",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledRed,						0, 255, 10, "" },
		{ "led_green",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledGreen,						0, 255, 0, "" },
		{ "led_blue",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledBlue,						0, 255, 0, "" },
		{ "led_mode",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledMode,						0, 255, 0, "" },

		{ "telem_sport",		typeUINT,  "telm", &mainConfig.telemConfig.telemSport,					0, TELEM_LAST, TELEM_OFF, "" },
		{ "telem_spek",	 		typeUINT,  "telm", &mainConfig.telemConfig.telemSpek,					0, TELEM_LAST, TELEM_OFF, "" },
		{ "telem_msp",	 		typeUINT,  "telm", &mainConfig.telemConfig.telemMsp,					0, TELEM_LAST, TELEM_OFF, "" },
		{ "telem_mavlink", 		typeUINT,  "telm", &mainConfig.telemConfig.telemMav,					0, TELEM_LAST, TELEM_OFF, "" },

		{ "gyro_rotation", 		typeUINT,  "gyro", &mainConfig.gyroConfig.gyroRotation,					0, CW315_INV, CW0, "" },
		{ "board_calibrated", 	typeUINT,  "gyro", &mainConfig.gyroConfig.boardCalibrated,				0, 1,  0, "" },
		{ "sml_board_rot_x", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[X],		0, 10, 0, "" },
		{ "sml_board_rot_y", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Y],		0, 10, 0, "" },
		{ "sml_board_rot_z", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Z], 		0, 10, 0, "" },
		{ "rf_loop_ctrl", 		typeUINT,  "gyro", &mainConfig.gyroConfig.loopCtrl, 					0, LOOP_UH32, LOOP_H32, "" },

		{ "yaw_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kp, 						0, 500, 160.00, "" }, //1000 18
		{ "roll_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kp, 						0, 500, 140.00, "" },
		{ "pitch_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kp, 					0, 500, 150.00, "" },

		{ "yaw_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].ki, 						0, 3000, 1200.00, "" }, //1000 14
		{ "roll_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].ki, 						0, 3000, 400.00, "" },
		{ "pitch_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].ki, 					0, 3000, 450.00, "" },

		{ "yaw_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kd, 						0, 3000, 1400.00, "" }, //1000000 114
		{ "roll_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kd, 						0, 3000, 800.00, "" },
		{ "pitch_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kd, 					0, 3000, 1000.00, "" },

		{ "yaw_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[YAW].wc, 						0, 32, 0, "" },
		{ "roll_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[ROLL].wc, 						0, 32, 0, "" },
		{ "pitch_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[PITCH].wc, 					0, 32, 0, "" },

		{ "yaw_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[YAW].ga, 						0, 32, 0, "" },
		{ "roll_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[ROLL].ga, 						0, 32, 0, "" },
		{ "pitch_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[PITCH].ga, 					0, 32, 0, "" },

		{ "slp", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].slp, 					0, 25, 20, "" },
		{ "sli", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].sli, 					0, 25, 20, "" },
		{ "sla", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].sla, 					0, 50, 40, "" },
		{ "sld", 				typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].sld, 					0, 0.9, 0.65, "" },

		{ "filter_mode0",		typeUINT,  "filt", &mainConfig.filterConfig[0].filterMod, 				0, 10, 1, "" },
		{ "filter_mode1",		typeUINT,  "filt", &mainConfig.filterConfig[1].filterMod, 				0, 10, 0, "" },
		{ "filter_mode2",		typeUINT,  "filt", &mainConfig.filterConfig[2].filterMod, 				0, 10, 2, "" },

		{ "yaw_quick", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.q, 				0, 100, 25.000, "" },
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

		{ "rx_protocol", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.rxProtcol, 				0, USING_RX_END-1, USING_SPEK_T, "" },
		{ "rx_usart", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.rxUsart, 				0, MAX_USARTS-1, ENUM_USART1, "" },

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
		{ "yaw_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[YAW], 		0, 100, 0, "" },
		{ "throttle_map", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[THROTTLE],	0, 100, 3, "" },
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

		{ "pitch_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[PITCH], 		0, EXPO_CURVE_END, FAST_EXPO, "" },
		{ "roll_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[ROLL], 		0, EXPO_CURVE_END, FAST_EXPO, "" },
		{ "yaw_curve", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[YAW], 			0, EXPO_CURVE_END, FAST_EXPO, "" },
		{ "throttle_curve", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[THROTTLE], 	0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux1_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX1], 		0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux2_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX2], 		0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux3_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX3], 		0, EXPO_CURVE_END, NO_EXPO, "" },
		{ "aux4_curve", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.useCurve[AUX4], 		0, EXPO_CURVE_END, NO_EXPO, "" },

		{ "pitch_expo", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[PITCH],		0, 100, 60, "" },
		{ "roll_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[ROLL],		0, 100, 60, "" },
		{ "yaw_expo", 			typeFLOAT, "rccf", &mainConfig.rcControlsConfig.curveExpo[YAW],			0, 100, 60, "" },
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
	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ss %s", inString);
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
			autoSaveTimer = InlineMillis();
			if ( (!strcmp(valueTable[x].group, "telm")) || (!strcmp(valueTable[x].group, "mixr")) || (!strcmp(valueTable[x].group, "gyro")) || (!strcmp(valueTable[x].group, "filt"))  || (!strcmp(valueTable[x].group, "rccf")) ) {
				resetBoard=1;
			}
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me %s=%s", inString, args);
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
		sprintf(rf_custom_out_buffer, "set %s=%d", valueTable[position].name, (int)*(uint32_t *)valueTable[position].ptr);
		RfCustomReplyBuffer(rf_custom_out_buffer);
		//snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "set %s=%d", valueTable[position].name, (int)*(uint32_t *)valueTable[position].ptr);
		break;


	case typeINT:
		sprintf(rf_custom_out_buffer, "set %s=%d", valueTable[position].name, (int)*(int32_t *)valueTable[position].ptr);
		RfCustomReplyBuffer(rf_custom_out_buffer);
		//snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "set %s=%d", valueTable[position].name, (int)*(int32_t *)valueTable[position].ptr);
		break;


	case typeFLOAT:
		ftoa(*(float *)valueTable[position].ptr, fString);
		StripSpaces(fString);
		sprintf(rf_custom_out_buffer, "set %s=%s", valueTable[position].name, fString);
		RfCustomReplyBuffer(rf_custom_out_buffer);
		//snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "set %s=%s", valueTable[position].name, fString);
		break;
	}

	//RfCustomReply(rf_custom_out_buffer);
}

void OutputVar(uint32_t position)
{
	char fString[20];

	switch (valueTable[position].type) {

	case typeUINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%d", valueTable[position].name, (int)*(uint32_t *)valueTable[position].ptr);
		break;


	case typeINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%d", valueTable[position].name, (int)*(int32_t *)valueTable[position].ptr);
		break;


	case typeFLOAT:
		ftoa(*(float *)valueTable[position].ptr, fString);
		StripSpaces(fString);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s=%s", valueTable[position].name, fString);
		break;
	}

	RfCustomReplyBuffer(rf_custom_out_buffer);
}
/**********************************************************************************************************************/


int RfCustomReply(char *rf_custom_out_buffer)
{

	if (disableSaving)
		return(0);

	if (sendReturn)
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "%s\n", rf_custom_out_buffer);

	unsigned char rfReplyBuffer[RF_BUFFER_SIZE];

	bzero((rfReplyBuffer+1), (sizeof(rfReplyBuffer)-1));

	rfReplyBuffer[0]=1;
	memcpy((char *)(rfReplyBuffer+1), rf_custom_out_buffer, RF_BUFFER_SIZE);

	USBD_HID_SendReport (&hUsbDeviceFS, rfReplyBuffer, HID_EPIN_SIZE);
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
		//one second more timeout
		if (Micros() - lastTimeMore < 1000000)
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
		//if (startCleanup)
		//	args[x] = 0;
	}


	for (x = 0; x < strlen(inString); x++)
		inString[x] = tolower((unsigned char)inString[x]);


	//ignore any string that starts with #
	if (inString[0] == '#')
		return;


	if (!strcmp("more", inString))
		{
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#nomore" );
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("polladc", inString))
		{
			if (HAL_ADC_PollForConversion(&adcHandle[board.boardADC[1].adcHandle], 10000) == HAL_OK)
			{
				snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me ADC: %lu", HAL_ADC_GetValue(&adcHandle[board.boardADC[1].adcHandle]) );
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
			else
			{
				RfCustomReplyBuffer("#me ADC ERROR");
			}
		}
	else if (!strcmp("idle", inString))
		{
			uint32_t motorToSpin = CONSTRAIN( atoi(args),0, MAX_MOTOR_NUMBER);
			DisarmBoard();
			SKIP_GYRO=1;

			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Spinning Motor %lu", motorToSpin );
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

			RfCustomReplyBuffer("#me RX Data has been faked");

		}
	else if (!strcmp("idlestop", inString))
		{
			DisarmBoard();
			ZeroActuators( 1000 );
			SKIP_GYRO=0;
			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "idlestop" );
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("error", inString))
		{
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%lu %lu", deviceWhoAmI, errorMask);
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("fakeflash", inString))
		{
			flashInfo.enabled = FLASH_ENABLED;
			//flashInfo.currentWriteAddress = atoi(args);
			flashCountdownFake = 100;

			snprintf( rf_custom_out_buffer, RF_BUFFER_SIZE, "#me It's a FAAAAKE" );
			RfCustomReplyBuffer(rf_custom_out_buffer);

		}
	else if (!strcmp("set", inString))
		{
			if (!SetVariable(args))
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Setting Not Found:%s", inString);
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rxdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#rx %u=%u", (volatile unsigned int)xx+1, (volatile unsigned int)(rxData[xx]));
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rcdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#rc %d=%d", (volatile int)xx+1, (volatile int)(trueRcCommandF[xx]*1000));
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rxrcdata", inString) || !strcmp("rcrxdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#rb %u=%u:%d", (volatile unsigned int)(xx+1), (volatile unsigned int)(rxData[xx]), (volatile int)(trueRcCommandF[xx]*1000));
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

			//todo: make a way to combine strings
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm pitch=%s\n#tm roll=%s\n#tm heading=%s", pitchString,rollString,yawString);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm ax=%s\n#tm ay=%s\n#tm az=%s", ax,ay,az);
			RfCustomReplyBuffer(rf_custom_out_buffer);
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#tm gx=%s\n#tm gy=%s\n#tm gz=%s", gx,gy,gz);
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me SBUS Defaults");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me SUMD Defaults");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me IBUS Defaults");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me CPPM Defaults");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();
		}
	else if (!strcmp("pidsd139", inString))
		{

			mainConfig.pidConfig[YAW].kp   = 305.00;
			mainConfig.pidConfig[ROLL].kp  = 110.00;
			mainConfig.pidConfig[PITCH].kp = 135.00;

			mainConfig.pidConfig[YAW].ki   = 475.00;
			mainConfig.pidConfig[ROLL].ki  = 370.00;
			mainConfig.pidConfig[PITCH].ki = 435.00;

			mainConfig.pidConfig[YAW].kd   = 2100.00;
			mainConfig.pidConfig[ROLL].kd  = 800.00;
			mainConfig.pidConfig[PITCH].kd = 1000.00;

			mainConfig.pidConfig[YAW].ga   = 3.00;
			mainConfig.pidConfig[ROLL].ga  = 3.00;
			mainConfig.pidConfig[PITCH].ga = 3.00;

			mainConfig.pidConfig[YAW].wc   = 4;
			mainConfig.pidConfig[ROLL].wc  = 4;
			mainConfig.pidConfig[PITCH].wc = 2;

			mainConfig.filterConfig[YAW].gyro.r   = 150.00;
			mainConfig.filterConfig[ROLL].gyro.r  = 100.00;
			mainConfig.filterConfig[PITCH].gyro.r = 100.00;

			mainConfig.filterConfig[YAW].gyro.q   = 7.000;
			mainConfig.filterConfig[ROLL].gyro.q  = 7.000;
			mainConfig.filterConfig[PITCH].gyro.q = 7.000;

			mainConfig.filterConfig[0].filterMod  = 0;
			mainConfig.filterConfig[1].filterMod  = 0;
			mainConfig.filterConfig[2].filterMod  = 1;

			mainConfig.filterConfig[YAW].kd.r     = 75.0;
			mainConfig.filterConfig[ROLL].kd.r    = 80.0;
			mainConfig.filterConfig[PITCH].kd.r   = 80.0;

			resetBoard = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Default PIDs");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();

		}
	else if (!strcmp("pidsdefault", inString))
		{

			mainConfig.pidConfig[YAW].kp          = 160.00;
			mainConfig.pidConfig[ROLL].kp         = 140.00;
			mainConfig.pidConfig[PITCH].kp        = 150.00;

			mainConfig.pidConfig[YAW].ki          = 1200.00;
			mainConfig.pidConfig[ROLL].ki         = 400.00;
			mainConfig.pidConfig[PITCH].ki        = 450.00;

			mainConfig.pidConfig[YAW].kd          = 1400.00;
			mainConfig.pidConfig[ROLL].kd         = 800.000;
			mainConfig.pidConfig[PITCH].kd        = 1000.00;

			mainConfig.pidConfig[YAW].ga          = 0.000;
			mainConfig.pidConfig[ROLL].ga         = 0.000;
			mainConfig.pidConfig[PITCH].ga        = 0.000;

			mainConfig.pidConfig[YAW].wc          = 0;
			mainConfig.pidConfig[ROLL].wc         = 0;
			mainConfig.pidConfig[PITCH].wc        = 0;

			mainConfig.filterConfig[YAW].gyro.r   = 88.000;
			mainConfig.filterConfig[ROLL].gyro.r  = 88.000;
			mainConfig.filterConfig[PITCH].gyro.r = 88.000;

			mainConfig.filterConfig[YAW].gyro.q   = 25.000;
			mainConfig.filterConfig[ROLL].gyro.q  = 60.000;
			mainConfig.filterConfig[PITCH].gyro.q = 60.000;

			mainConfig.filterConfig[0].filterMod  = 1;
			mainConfig.filterConfig[1].filterMod  = 0;
			mainConfig.filterConfig[2].filterMod  = 2;

			mainConfig.filterConfig[YAW].kd.r     = 90.000;
			mainConfig.filterConfig[ROLL].kd.r    = 90.000;
			mainConfig.filterConfig[PITCH].kd.r   = 90.000;

			resetBoard = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Default PIDs");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();

		}
	else if (!strcmp("pidsrs2k", inString))
		{

			mainConfig.pidConfig[YAW].kp          = 160.00;
			mainConfig.pidConfig[ROLL].kp         = 140.00;
			mainConfig.pidConfig[PITCH].kp        = 150.00;

			mainConfig.pidConfig[YAW].ki          = 1200.00;
			mainConfig.pidConfig[ROLL].ki         = 400.00;
			mainConfig.pidConfig[PITCH].ki        = 450.00;

			mainConfig.pidConfig[YAW].kd          = 1400.00;
			mainConfig.pidConfig[ROLL].kd         = 800.000;
			mainConfig.pidConfig[PITCH].kd        = 1000.00;

			mainConfig.pidConfig[YAW].ga          = 0.00;
			mainConfig.pidConfig[ROLL].ga         = 0.000;
			mainConfig.pidConfig[PITCH].ga        = 0.000;

			mainConfig.pidConfig[YAW].wc          = 0;
			mainConfig.pidConfig[ROLL].wc         = 0;
			mainConfig.pidConfig[PITCH].wc        = 0;

			mainConfig.filterConfig[YAW].gyro.r   = 95.000;
			mainConfig.filterConfig[ROLL].gyro.r  = 88.000;
			mainConfig.filterConfig[PITCH].gyro.r = 88.000;

			mainConfig.filterConfig[YAW].gyro.q   = 25.000;
			mainConfig.filterConfig[ROLL].gyro.q  = 60.000;
			mainConfig.filterConfig[PITCH].gyro.q = 60.000;

			mainConfig.filterConfig[0].filterMod  = 1;
			mainConfig.filterConfig[1].filterMod  = 1;
			mainConfig.filterConfig[2].filterMod  = 1;

			mainConfig.filterConfig[YAW].kd.r     = 90.000;
			mainConfig.filterConfig[ROLL].kd.r    = 90.000;
			mainConfig.filterConfig[PITCH].kd.r   = 90.000;

			resetBoard = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me RS2K PIDs");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me SPEK Defaults");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
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
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Error");
					RfCustomReplyBuffer(rf_custom_out_buffer);
					break;
			}

			SetRxDefaults(protocol, usart);
			SetMode(M_ARMED, 4, 50, 100);
			resetBoard = 1;
			mainConfig.rcControlsConfig.rcCalibrated = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me DSM2 Defaults");
			RfCustomReplyBuffer(rf_custom_out_buffer);

			SaveAndSend();
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

				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s", FULL_VERSION_STRING);
				RfCustomReply(rf_custom_out_buffer);
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
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me No Arguments Found For:%s", args);
				RfCustomReplyBuffer(rf_custom_out_buffer);

			}
			//RfCustomReplyBuffer("#RFEND");
			//snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#RFEND");
			//RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("eraseallflash", inString))
		{

			if (flashInfo.enabled)
			{

				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Erasing Flash");
				RfCustomReplyBuffer(rf_custom_out_buffer);

				if (MassEraseDataFlash(1))
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Complete");
					RfCustomReplyBuffer(rf_custom_out_buffer);
				}
				else
				{
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Failed");
					RfCustomReplyBuffer(rf_custom_out_buffer);
				}

			}
			else
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Chip Not Detected");
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}

		}
	else if (!strcmp("eraseflash", inString))
		{
			if (flashInfo.enabled)
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Erasing Flash");
				RfCustomReplyBuffer(rf_custom_out_buffer);

				if (((float)(flashInfo.currentWriteAddress) / (float)flashInfo.totalSize) > 0.85) {
					if (MassEraseDataFlash(1))
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Complete");
						RfCustomReplyBuffer(rf_custom_out_buffer);
					}
					else
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Failed");
						RfCustomReplyBuffer(rf_custom_out_buffer);
					}
				} else {
					if (MassEraseDataFlashByPage(1))
					{
						snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Erase Complete");
						RfCustomReplyBuffer(rf_custom_out_buffer);
					}
				}

			}
			else
			{
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Chip Not Detected");
				RfCustomReplyBuffer(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("dlflstatusdump", inString))
		{
			DlflStatusDump();
		}
	else if (!strcmp("readflash", inString))
		{
			uint32_t smallerPointer;

			if (flashInfo.enabled) {

				args = StripSpaces(args);

				if ( M25p16ReadPage( atoi(args), flashInfo.buffer[0].txBuffer, flashInfo.buffer[0].rxBuffer) )
				{

					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					sendReturn = 0;
					for (uint32_t x=0;x<RF_BUFFER_SIZE;x++)
					{
						rf_custom_out_buffer[smallerPointer++] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
					}
					RfCustomReply(rf_custom_out_buffer);
					sendReturn = 1;
					return;

				}
				else
				{

					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Read Failed");
					RfCustomReplyBuffer(rf_custom_out_buffer);

				}

			}
			else
			{

				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Flash Chip Not Detected");
				RfCustomReplyBuffer(rf_custom_out_buffer);

			}

		}
	else if (!strcmp("version", inString))
		{
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%s", FULL_VERSION_STRING);
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
			RfCustomReply("#me Rebooting");
			SystemReset();
			return;
		}
	else if (!strcmp("resetdfu", inString)  || !strcmp("rebootdfu", inString))
		{
			RfCustomReply("#me Rebooting Into DFU");
			SystemResetToDfuBootloader();
			return;
		}
	else if (!strcmp("resetconfig", inString))
		{

			resetBoard = 1;

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Resetting Config");
			RfCustomReplyBuffer(rf_custom_out_buffer);
			GenerateConfig();

			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Config Reset");
			RfCustomReplyBuffer(rf_custom_out_buffer);
			SaveAndSend();

		}
	else if (!strcmp("binds", inString))
		{
			sendSpektrumBind();
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Binding Serial");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if ( (!strcmp("bind9", inString)) || (!strcmp("bind", inString)) )
		{
			mainConfig.rcControlsConfig.bind = 9;
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			rtc_write_backup_reg(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			SaveAndSend();
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Binding 9");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("bind5", inString))
		{
			mainConfig.rcControlsConfig.bind = 5;
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			rtc_write_backup_reg(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			SaveAndSend();
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Binding 5");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("bind3", inString))
		{
			mainConfig.rcControlsConfig.bind = 3;
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			rtc_write_backup_reg(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			SaveAndSend();
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#me Binding 3");
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}
	else if (!strcmp("rebootrfbl", inString) || !strcmp("resetrfbl", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RFBL_COMMAND);
			RfCustomReply("#me Rebooting Into RFBL");
			DelayMs(100);
			SystemReset();
			return;
		}
	else if (!strcmp("rebootrecovery", inString) || !strcmp("resetrecovery", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RECOVERY_COMMAND);
			RfCustomReply("#me Rebooting Into Recovery");
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
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ms Unknown Command:%s", args);
			RfCustomReplyBuffer(rf_custom_out_buffer);
		}

	SendRfCustomReplyBuffer();
	lastTimeMore=Micros();

}

void SaveAndSend(void)
{
	if (disableSaving)
		return;

	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ms Saving");
	RfCustomReplyBuffer(rf_custom_out_buffer);
	SaveConfig(ADDRESS_CONFIG_START);
	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "#ms Save Complete");
	RfCustomReplyBuffer(rf_custom_out_buffer);
}

void DlflStatusDump(void)
{
	sprintf(rf_custom_out_buffer, "#fl size=%u\n#fl total=%u", (unsigned int)(flashInfo.currentWriteAddress), (unsigned int)(flashInfo.totalSize));
	RfCustomReplyBuffer(rf_custom_out_buffer);
}



int RfCustomReplyBuffer(char *rfCustomSendBufferAdder)
{

	//add rfCustomSendBufferAdder to rfCustomSendBuffer and add a \n to the end
	snprintf(rfCustomSendBuffer+rfCustomReplyBufferPointer, LARGE_RF_BUFFER_SIZE-rfCustomReplyBufferPointer, "%s\n", rfCustomSendBufferAdder);

	rfCustomReplyBufferPointer += strlen(rfCustomSendBufferAdder) + 1; //adding a \n

	return(0);

}

int SendRfCustomReplyBuffer(void)
{

	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE+1, "%s", rfCustomSendBuffer+rfCustomReplyBufferPointerSent);
	RfCustomReply(rf_custom_out_buffer);
	rfCustomReplyBufferPointerSent +=RF_BUFFER_SIZE;

	if (rfCustomReplyBufferPointerSent >= rfCustomReplyBufferPointer)
	{
		rfCustomReplyBufferPointerSent=0;
		rfCustomReplyBufferPointer=0;
	}


	return(0);
}

