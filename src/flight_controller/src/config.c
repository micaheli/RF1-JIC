#include "includes.h"


// use variable record but instead of storing address of variable, store offset based on address of field, that way it works with the record loaded from file

main_config mainConfig;
uint32_t resetBoard = 0;
char rf_custom_out_buffer[RF_BUFFER_SIZE];

static uint32_t ValidateConfig (uint32_t addresConfigStart);
static void     SetValueOrString(uint32_t position, char *value);
static void     SetValue(uint32_t position, char *value);

static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void EncodeBlock( unsigned char *in, unsigned char *out, int len )
{
   out[0] = (unsigned char) cb64[ (int)(in[0] >> 2) ];
   out[1] = (unsigned char) cb64[ (int)(((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)) ];
   out[2] = (unsigned char) (len > 1 ? cb64[ (int)(((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6)) ] : '=');
   out[3] = (unsigned char) (len > 2 ? cb64[ (int)(in[2] & 0x3f) ] : '=');
}

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
		{"MIXER_X1234I",  MIXER_X1234I },
		{"MIXER_X1234RY", MIXER_X1234RY },
		{"MIXER_X1234",   MIXER_X1234 },
		{"MIXER_CUSTOM",  MIXER_CUSTOM },

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
		{"USING_SPEKTRUM_R",       USING_SPEKTRUM_R},
		{"USING_SPEKTRUM_T",       USING_SPEKTRUM_T},
		{"USING_SBUS_R",           USING_SBUS_R},
		{"USING_SBUS_T",           USING_SBUS_T},
		{"USING_SUMD_R",           USING_SUMD_R},
		{"USING_SUMD_T",           USING_SUMD_T},
		{"USING_IBUS_R",           USING_IBUS_R},
		{"USING_IBUS_T",           USING_IBUS_T},
		{"USING_PPM_R",            USING_PPM_R},
		{"USING_PPM_T",            USING_PPM_T},

};

const config_variables_rec valueTable[] = {

		{ "mixer_type", 		typeUINT,  "mixr", &mainConfig.mixerConfig.mixerType,					0, MIXER_END, MIXER_X1234, "" },
		{ "esc_protocol", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escProtcol,					0, ESC_PROTOCOL_END, ESC_MULTISHOT, "" },
		{ "esc_frequency", 		typeUINT,  "mixr", &mainConfig.mixerConfig.escUpdateFrequency,			0, 32000, 32000, "" },
		{ "idle_percent", 		typeFLOAT, "mixr", &mainConfig.mixerConfig.idlePercent,					0, 15.0, 5, "" },
		{ "motor_mixer", 		typeUINT,  "mixr", &mainConfig.mixerConfig.motorMixer,					0, 2, 0, "" },
		{ "mout1", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[0],				0, 9, 0, "" },
		{ "mout2", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[1],				0, 9, 1, "" },
		{ "mout3", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[2],				0, 9, 2, "" },
		{ "mout4", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[3],				0, 9, 3, "" },
		{ "mout5", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[4],				0, 9, 4, "" },
		{ "mout6", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[5],				0, 9, 5, "" },
		{ "mout7", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[6],				0, 9, 6, "" },
		{ "mout8", 				typeUINT,  "mixr", &mainConfig.mixerConfig.motorOutput[7],				0, 9, 7, "" },

		{ "led_count",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledCount,						0, WS2812_MAX_LEDS, 8, "" },
		{ "led_red",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledRed,						0, 255, 1, "" },
		{ "led_green",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledGreen,						0, 255, 0, "" },
		{ "led_blue",	 		typeUINT,  "leds", &mainConfig.ledConfig.ledBlue,						0, 255, 0, "" },

		{ "telem_sport",		typeUINT,  "telm", &mainConfig.telemConfig.telemSport,					0, TELEM_LAST, TELEM_OFF, "" },
		{ "telem_spek",	 		typeUINT,  "telm", &mainConfig.telemConfig.telemSpek,					0, TELEM_LAST, TELEM_OFF, "" },
		{ "telem_msp",	 		typeUINT,  "telm", &mainConfig.telemConfig.telemMsp,					0, TELEM_LAST, TELEM_OFF, "" },
		{ "telem_mavlink", 		typeUINT,  "telm", &mainConfig.telemConfig.telemMav,					0, TELEM_LAST, TELEM_OFF, "" },

		{ "gyro_rotation", 		typeUINT,  "gyro", &mainConfig.gyroConfig.gyroRotation,					0, 12, CW0, "" },
		{ "board_calibrated", 	typeUINT,  "gyro", &mainConfig.gyroConfig.boardCalibrated,				0, 1,  0, "" },
		{ "sml_board_rot_x", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[X],		0, 10, 0, "" },
		{ "sml_board_rot_y", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Y],		0, 10, 0, "" },
		{ "sml_board_rot_z", 	typeINT,   "gyro", &mainConfig.gyroConfig.minorBoardRotation[Z], 		0, 10, 0, "" },
		{ "rf_loop_ctrl", 		typeUINT,  "gyro", &mainConfig.gyroConfig.loopCtrl, 					0, LOOP_UH32, LOOP_UH32, "" },
		{ "gyro_filter_type",	typeUINT,  "gyro", &mainConfig.gyroConfig.filterTypeGyro, 				0, 2, 0, "" },
		{ "kd_filter_type",		typeUINT,  "gyro", &mainConfig.gyroConfig.filterTypeKd, 				0, 2, 2, "" },

		{ "yaw_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kp, 						0, 300, 250.00, "" }, //1000 18
		{ "roll_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kp, 						0, 300, 150.00, "" },
		{ "pitch_kp", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kp, 					0, 300, 200.00, "" },
		{ "yaw_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].ki, 						0, 300, 600.00, "" }, //1000 14
		{ "roll_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].ki, 						0, 300, 500.00, "" },
		{ "pitch_ki", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].ki, 					0, 300, 550.00, "" },
		{ "yaw_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[YAW].kd, 						0, 300, 700.00, "" }, //1000000 114
		{ "roll_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[ROLL].kd, 						0, 300, 700.00, "" },
		{ "pitch_kd", 			typeFLOAT, "pids", &mainConfig.pidConfig[PITCH].kd, 					0, 300, 800.00, "" },
		{ "yaw_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[YAW].wc, 						0, 300, 0, "" },
		{ "roll_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[ROLL].wc, 						0, 300, 0, "" },
		{ "pitch_wc", 			typeUINT,  "pids", &mainConfig.pidConfig[PITCH].wc, 					0, 300, 0, "" },

		{ "yaw_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[YAW].ga, 						0, 32, 0, "" },
		{ "roll_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[ROLL].ga, 						0, 32, 0, "" },
		{ "pitch_ga", 			typeUINT,  "pids", &mainConfig.pidConfig[PITCH].ga, 					0, 32, 0, "" },

		{ "slp", 				typeFLOAT,  "pids", &mainConfig.pidConfig[PITCH].slp, 					0, 32, 20, "" },
		{ "sli", 				typeFLOAT,  "pids", &mainConfig.pidConfig[PITCH].sli, 					0, 32, 20, "" },
		{ "sla", 				typeFLOAT,  "pids", &mainConfig.pidConfig[PITCH].sla, 					0, 32, 40, "" },
		{ "sld", 				typeFLOAT,  "pids", &mainConfig.pidConfig[PITCH].sld, 					0, 32, 0.65, "" },

		{ "yaw_quick", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.q, 				0, 10, 30.000, "" },
		{ "yaw_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.r, 				0, 10, 88.000, "" },
		{ "yaw_press", 			typeFLOAT, "filt", &mainConfig.filterConfig[YAW].gyro.p, 				0, 10, 0.0000, "" },
		{ "roll_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.q, 				0, 10, 30.000, "" },
		{ "roll_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.r, 				0, 10, 88.000, "" },
		{ "roll_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].gyro.p, 				0, 10, 0.0000, "" },
		{ "pitch_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.q, 				0, 10, 30.000, "" },
		{ "pitch_rap", 			typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.r, 				0, 10, 88.000, "" },
		{ "pitch_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].gyro.p, 				0, 10, 0.0000, "" },

		{ "yaw_kd_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kd.q, 					0, 10, 0.0000, "" },
		{ "yaw_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kd.r, 					0, 10, 90.000, "" },
		{ "yaw_kd_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kd.p, 					0, 10, 0.0000, "" },
		{ "roll_kd_quick", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kd.q, 				0, 10, 0.0000, "" },
		{ "roll_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kd.r, 				0, 10, 90.000, "" },
		{ "roll_kd_press", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kd.p, 				0, 10, 0.0000, "" },
		{ "pitch_kd_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kd.q, 				0, 10, 0.0000, "" },
		{ "pitch_kd_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kd.r, 				0, 10, 90.000, "" },
		{ "pitch_kd_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kd.p, 				0, 10, 0.0000, "" },

		{ "yaw_kd_lpf", 		typeFLOAT, "filt", &mainConfig.filterConfig[YAW].kdBq.lpfHz, 			0, 200, 68.00, "" },
		{ "roll_kd_lpf", 		typeFLOAT, "filt", &mainConfig.filterConfig[ROLL].kdBq.lpfHz, 			0, 200, 68.00, "" },
		{ "pitch_kd_lpf", 		typeFLOAT, "filt", &mainConfig.filterConfig[PITCH].kdBq.lpfHz, 			0, 200, 68.00, "" },


		{ "x_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.q, 				0, 10, 2.0000, "" },
		{ "x_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.r, 				0, 10, 025.00, "" },
		{ "x_vector_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCX].acc.p, 				0, 10, 0.0150, "" },
		{ "y_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.q, 				0, 10, 2.0000, "" },
		{ "y_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.r, 				0, 10, 025.00, "" },
		{ "y_vector_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCY].acc.p, 				0, 10, 0.0500, "" },
		{ "z_vector_quick", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.q, 				0, 10, 2.0000, "" },
		{ "z_vector_rap", 		typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.r, 				0, 10, 025.00, "" },
		{ "z_vector_press", 	typeFLOAT, "filt", &mainConfig.filterConfig[ACCZ].acc.p, 				0, 10, 0.0500, "" },

		{ "rx_protocol", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.rxProtcol, 				0, 10, USING_SPEKTRUM_T, "" },
		{ "rx_usart", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.rxUsart, 				0, MAX_USARTS-1, ENUM_USART1, "" },

		{ "pitch_deadband", 	typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[PITCH], 		0, 0.1, 0.003, "" },
		{ "roll_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[ROLL], 		0, 0.1, 0.003, "" },
		{ "yaw_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[YAW], 			0, 0.1, 0.003, "" },
		{ "throttle_deadband", 	typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[THROTTLE], 	0, 0.1, 0, "" },
		{ "aux1_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX1], 		0, 0.1, 0, "" },
		{ "aux2_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX2], 		0, 0.1, 0, "" },
		{ "aux3_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX3], 		0, 0.1, 0, "" },
		{ "aux4_deadband", 		typeFLOAT, "rccf", &mainConfig.rcControlsConfig.deadBand[AUX4], 		0, 0.1, 0, "" },

		//spektrum is 1024
		//sbus is 998
		{ "pitch_midrc",		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[PITCH], 			0, 2148, 1024, "" },
		{ "roll_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[ROLL], 			0, 2148, 1024, "" },
		{ "yaw_midrc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[YAW], 			0, 2148, 1024, "" },
		{ "throttle_midrc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[THROTTLE],		0, 2148, 1024, "" },
		{ "aux1_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX1], 			0, 2148, 1024, "" },
		{ "aux2_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX2], 			0, 2148, 1024, "" },
		{ "aux3_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX3], 			0, 2148, 1024, "" },
		{ "aux4_midrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.midRc[AUX4], 			0, 2148, 1024, "" },

		{ "pitch_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[PITCH], 			0, 2148, 0, "" },
		{ "roll_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[ROLL], 			0, 2148, 0, "" },
		{ "yaw_minrc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[YAW], 			0, 2148, 0, "" },
		{ "throttle_minrc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[THROTTLE], 		0, 2148, 0, "" },
		{ "aux1_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX1], 			0, 2148, 0, "" },
		{ "aux2_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX2], 			0, 2148, 0, "" },
		{ "aux3_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX3], 			0, 2148, 0, "" },
		{ "aux4_minrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.minRc[AUX4], 			0, 2148, 0, "" },

		{ "pitch_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[PITCH], 			0, 2148, 2048, "" },
		{ "roll_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[ROLL], 			0, 2148, 2048, "" },
		{ "yaw_maxrc", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[YAW], 			0, 2148, 2048, "" },
		{ "throttle_maxrc", 	typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[THROTTLE], 		0, 2148, 2048, "" },
		{ "aux1_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX1], 			0, 2148, 2048, "" },
		{ "aux2_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX2], 			0, 2148, 2048, "" },
		{ "aux3_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX3], 			0, 2148, 2048, "" },
		{ "aux4_maxrc", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.maxRc[AUX4], 			0, 2148, 2048, "" },

		{ "pitch_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[PITCH], 		0, 1000, 2, "" },
		{ "roll_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[ROLL], 		0, 1000, 1, "" },
		{ "yaw_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[YAW], 		0, 1000, 0, "" },
		{ "throttle_map", 		typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[THROTTLE],	0, 1000, 3, "" },
		{ "aux1_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX1], 		0, 1000, 4, "" },
		{ "aux2_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX2], 		0, 1000, 5, "" },
		{ "aux3_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX3], 		0, 1000, 6, "" },
		{ "aux4_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX4], 		0, 1000, 7, "" },
		{ "aux5_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX5], 		0, 1000, 100, "" },
		{ "aux6_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX6], 		0, 1000, 100, "" },
		{ "aux7_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX7], 		0, 1000, 100, "" },
		{ "aux8_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX8], 		0, 1000, 100, "" },
		{ "aux9_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX9], 		0, 1000, 100, "" },
		{ "aux10_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX10], 		0, 1000, 100, "" },
		{ "aux11_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX11], 		0, 1000, 100, "" },
		{ "aux12_map", 			typeUINT,  "rccf", &mainConfig.rcControlsConfig.channelMap[AUX12], 		0, 1000, 100, "" },

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

		{ "pitch_rate", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[PITCH],			0, 1400, 400, "" },
		{ "roll_rate", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[ROLL],			0, 1400, 400, "" },
		{ "yaw_rate", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.rates[YAW],				0, 1400, 400, "" },

		{ "pitch_acrop", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[PITCH],		0, 4, 1.4, "" },
		{ "roll_acrop", 		typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[ROLL],			0, 4, 1.4, "" },
		{ "yaw_acrop", 			typeFLOAT, "rate", &mainConfig.rcControlsConfig.acroPlus[YAW],			0, 4, 1.4, "" }

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

	if (resetBoard) {

		//This function does a shotgun startup of the flight code.
		InitFlight();

	}

	mainConfig.version  = CONFIG_VERSION;
	mainConfig.size     = sizeof(main_config);
	mainConfig.czechsum = CalculateCzechsum((const uint8_t *) &mainConfig, sizeof(main_config));

	EraseFlash(addresConfigStart, addresConfigStart+sizeof(main_config));
	PrepareFlash();
	for (addressOffset = 0; addressOffset < sizeof(main_config); addressOffset += 4) {
		WriteFlash(*(uint32_t *) ((char *) &mainConfig + addressOffset), addresConfigStart+addressOffset );
	}
	FinishFlash();

}

uint8_t CalculateCzechsum(const uint8_t *data, uint32_t length)
{

	uint8_t czechsum = 0;
    const uint8_t *byteOffset;

    for (byteOffset = data; byteOffset < (data + length); byteOffset++)
    	czechsum ^= *byteOffset;

    return(czechsum);

}

static uint32_t ValidateConfig (uint32_t addresConfigStart)
{

	const main_config *temp = (main_config *) addresConfigStart; //ADDRESS_FLASH_START;
	uint8_t czechsum = 0;

	if (temp->size != sizeof(main_config))
	    return (0);

	if (CONFIG_VERSION != temp->version)
		return (0);

	czechsum = CalculateCzechsum((const uint8_t *) temp, sizeof(main_config));
	if (czechsum != 0)
		return (1);
	    //todo: fix stupid checksum return (0);

	return (1);

}

void LoadConfig (uint32_t addresConfigStart)
{
	if (ValidateConfig(addresConfigStart) ) {
		memcpy(&mainConfig, (char *) addresConfigStart, sizeof(main_config));
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
	snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "STAT:%s\n", inString);
	RfCustomReply(rf_custom_out_buffer);
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

	//TODO: Remove
	if (!strcmp("board_calibrated", inString)) {
		memcpy(rf_custom_out_buffer, "board_calibrated cannot be set manually. Please run the board calibration routine.\0", sizeof("board_calibrated cannot be set manually. Please run the board calibration routine.\0"));
		RfCustomReply(rf_custom_out_buffer);
		return (0);
	}


	for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
	{
		if (!strcmp(valueTable[x].name, inString))
		{
			SetValueOrString(x, args);
			autoSaveTimer = InlineMillis();
			if ( (!strcmp(valueTable[x].group, "telm")) || (!strcmp(valueTable[x].group, "mixr")) || (!strcmp(valueTable[x].group, "gyro")) || (!strcmp(valueTable[x].group, "filt"))  || (!strcmp(valueTable[x].group, "rccf")) ) {
				resetBoard=1;
			}
			return (1);
		}
	}
	return (0);
}




/**********************************************************************************************************/
void OutputVarSet(uint32_t position)
{
	char fString[20];

	bzero(rf_custom_out_buffer, RF_BUFFER_SIZE);
	switch (valueTable[position].type) {

	case typeUINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "set %s=%d\n", valueTable[position].name, (int)*(uint32_t *)valueTable[position].ptr);
		break;


	case typeINT:
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "set %s=%d\n", valueTable[position].name, (int)*(int32_t *)valueTable[position].ptr);
		break;


	case typeFLOAT:
		ftoa(*(float *)valueTable[position].ptr, fString);
		StripSpaces(fString);
		snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE-1, "set %s=%s\n", valueTable[position].name, fString);
		break;
	}

	RfCustomReply(rf_custom_out_buffer);
}

void OutputVar(uint32_t position)
{
	char fString[20];

	bzero(rf_custom_out_buffer, RF_BUFFER_SIZE);
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

	RfCustomReply(rf_custom_out_buffer);
}
/**********************************************************************************************************************/


int RfCustomReply(char *rf_custom_out_buffer)
{

	uint32_t forCounter;

	unsigned char rfReplyBuffer[RF_BUFFER_SIZE];

	bzero((rfReplyBuffer+1), (sizeof(rfReplyBuffer)-1));

	rfReplyBuffer[0]=1;
	memcpy((char *)(rfReplyBuffer+1), rf_custom_out_buffer, RF_BUFFER_SIZE);

	for (forCounter = 0; forCounter < 3000; forCounter++) {
		if (hidToPcReady) {
			USBD_HID_SendReport (&hUsbDeviceFS, rfReplyBuffer, HID_EPIN_SIZE);
			hidToPcReady = 0;
			return(1);
		}
		DelayMs(1); //wait 10 ms max
	}
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

	if (originalString[1] == 77) {
		inString = originalString;
	} else
		inString = CleanupString(inString);

	inStringLength = strlen(inString);

	for (x = 0; x < inStringLength; x++) {
		if ( (inString[x] == ' ') || (inString[x] == 220) )
			break;
	}

	if (inStringLength > x) {
		args = inString + x + 1;
	}

	inString[x] = 0;


	args = CleanupString(args);

	inStringLength = strlen(args);

	for (x = 0; x < inStringLength; x++) {
		if ( (inString[x] == ' ') || (inString[x] == 220) ) {
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

	if (!strcmp("set", inString))
		{
			if (SetVariable(args)) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "variablesetsuccess:%s", args);
				RfCustomReply(rf_custom_out_buffer);
			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "noargumentsfoundforargument:%s", args);
				RfCustomReply(rf_custom_out_buffer);
			}

		}
	else if (!strcmp("rxdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (volatile unsigned int)(rxData[xx]));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rcdata", inString))
		{
			for (uint32_t xx = 0; xx < MAXCHANNELS;xx++) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%d", (volatile int)(trueRcCommandF[xx]*1000));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("rcboth", inString))
		{
			//for (uint32_t xx = 0; xx < MAXCHANNELS;xx++) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u : %d", (volatile unsigned int)(rxData[0]), (volatile int)(trueRcCommandF[0]*1000));
				RfCustomReply(rf_custom_out_buffer);
			//}
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

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "telemstarted\npitch=%s\nroll=%s\nheading=%s\n", pitchString,rollString,yawString);
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "ax=%s\nay=%s\naz=%s\n", ax,ay,az);
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "gx=%s\ngy=%s\ngz=%s\ntelemcomplete\n", gx,gy,gz);
			RfCustomReply(rf_custom_out_buffer);

		}
	else if (!strcmp("sbus_r1", inString) || !strcmp("sbus_r3", inString) || !strcmp("sbus_r4", inString) || !strcmp("sbus_t1", inString) || !strcmp("sbus_t3", inString) || !strcmp("sbus_t4", inString))
		{
			mainConfig.rcControlsConfig.midRc[PITCH]         = 985;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 985;
			mainConfig.rcControlsConfig.midRc[YAW]           = 985;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 985;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 985;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 985;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 985;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 985;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 172;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 172;
			mainConfig.rcControlsConfig.minRc[YAW]           = 172;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 172;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 1811;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 172;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 0;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 0;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 1811;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 1811;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 1811;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 1811;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 172;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 1811;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1000000;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1000000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 100; //junk channel

			mainConfig.rcControlsConfig.rcCalibrated         = 1;

			if(!strcmp("sbus_r1", inString))
			{
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SBUS_R;       //this is used by serial.c
			}
			if(!strcmp("sbus_r3", inString))
			{
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SBUS_R;       //this is used by serial.c
			}
			if(!strcmp("sbus_r4", inString))
			{
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SBUS_R;       //this is used by serial.c
			}
			if(!strcmp("sbus_t1", inString))
			{
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SBUS_T; //this is used by serial.c
			}
			if(!strcmp("sbus_t3", inString))
			{
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SBUS_T; //this is used by serial.c
			}
			if(!strcmp("sbus_t4", inString))
			{
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SBUS_T; //this is used by serial.c
			}

			resetBoard = 1;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "settingdefaultstosbus\n", sizeof("settingdefaultstosbus\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("sumd_t1", inString) || !strcmp("sumd_t3", inString) || !strcmp("sumd_t4", inString) || !strcmp("sumd_r1", inString) || !strcmp("sumd_r3", inString) || !strcmp("sumd_r3", inString))
		{
			mainConfig.rcControlsConfig.midRc[PITCH]         = 12000;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 12000;
			mainConfig.rcControlsConfig.midRc[YAW]           = 12000;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 12000;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 12000;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 12000;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 12000;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 12000;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 8000;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 16000;
			mainConfig.rcControlsConfig.minRc[YAW]           = 16000;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 16000;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 8000;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 8000;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 0;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 0;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 16000;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 8000;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 8000;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 8000;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 16000;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 16000;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1000000;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1000000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 100; //junk channel

			mainConfig.rcControlsConfig.rcCalibrated         = 1;

			if(!strcmp("sumd_t1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SUMD_T; //this is used by serial.c
			}
			if(!strcmp("sumd_t3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SUMD_T; //this is used by serial.c
			}
			if(!strcmp("sumd_t4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SUMD_T; //this is used by serial.c
			}
			if(!strcmp("sumd_r1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SUMD_R; //this is used by serial.c
			}
			if(!strcmp("sumd_r3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SUMD_R; //this is used by serial.c
			}
			if(!strcmp("sumd_r4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SUMD_R; //this is used by serial.c
			}

			resetBoard = 1;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "settingdefaultstosumd\n", sizeof("settingdefaultstosumd\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("ibus_t1", inString) || !strcmp("ibus_t3", inString) || !strcmp("ibus_t4", inString) || !strcmp("ibus_r1", inString) || !strcmp("ibus_r3", inString) || !strcmp("ibus_r3", inString))
		{
			mainConfig.rcControlsConfig.midRc[PITCH]         = 1500;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 1500;
			mainConfig.rcControlsConfig.midRc[YAW]           = 1500;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1500;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 1500;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 1000;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 1000;
			mainConfig.rcControlsConfig.minRc[YAW]           = 1000;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 1000;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 0;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 0;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2000;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2000;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2000;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1000000;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1000000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 3;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 2;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 1;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 100; //junk channel

			mainConfig.rcControlsConfig.rcCalibrated         = 1;

			if(!strcmp("ibus_t1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_IBUS_T; //this is used by serial.c
			}
			if(!strcmp("ibus_t3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_IBUS_T; //this is used by serial.c
			}
			if(!strcmp("ibus_t4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_IBUS_T; //this is used by serial.c
			}
			if(!strcmp("ibus_r1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_IBUS_R; //this is used by serial.c
			}
			if(!strcmp("ibus_r3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_IBUS_R; //this is used by serial.c
			}
			if(!strcmp("ibus_r4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_IBUS_R; //this is used by serial.c
			}

			resetBoard = 1;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "settingdefaultstoibus\n", sizeof("settingdefaultstoibus\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("ppm_t1", inString) || !strcmp("ppm_t3", inString) || !strcmp("ppm_t4", inString) || !strcmp("ppm_r1", inString) || !strcmp("ppm_r3", inString) || !strcmp("ppm_r4", inString))
		{
			mainConfig.rcControlsConfig.midRc[PITCH]         = 1500;
			mainConfig.rcControlsConfig.midRc[ROLL]          = 1500;
			mainConfig.rcControlsConfig.midRc[YAW]           = 1500;
			mainConfig.rcControlsConfig.midRc[THROTTLE]      = 1500;
			mainConfig.rcControlsConfig.midRc[AUX1]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX2]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX3]          = 1500;
			mainConfig.rcControlsConfig.midRc[AUX4]          = 1500;

			mainConfig.rcControlsConfig.minRc[PITCH]         = 1000;
			mainConfig.rcControlsConfig.minRc[ROLL]          = 1000;
			mainConfig.rcControlsConfig.minRc[YAW]           = 1000;
			mainConfig.rcControlsConfig.minRc[THROTTLE]      = 1000;
			mainConfig.rcControlsConfig.minRc[AUX1]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX2]          = 1000;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 0;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 0;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2000;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2000;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2000;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 2000;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1000000;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1000000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 100; //junk channel

			mainConfig.rcControlsConfig.rcCalibrated         = 1;

			if(!strcmp("ppm_t1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_PPM_T; //this is used by serial.c
			}
			if(!strcmp("ppm_t3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_PPM_T; //this is used by serial.c
			}
			if(!strcmp("ppm_t4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_PPM_T; //this is used by serial.c
			}
			if(!strcmp("ppm_r1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_PPM_R; //this is used by serial.c
			}
			if(!strcmp("ppm_r3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_PPM_R; //this is used by serial.c
			}
			if(!strcmp("ppm_r4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_PPM_R; //this is used by serial.c
			}

			resetBoard = 1;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "settingdefaultstoppm\n", sizeof("settingdefaultstoppm\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("pidsdefault", inString))
		{
			mainConfig.pidConfig[YAW].kp   = 250.00;
			mainConfig.pidConfig[ROLL].kp  = 150.00;
			mainConfig.pidConfig[PITCH].kp = 200.00;

			mainConfig.pidConfig[YAW].ki   = 600.00;
			mainConfig.pidConfig[ROLL].ki  = 500.00;
			mainConfig.pidConfig[PITCH].ki = 550.00;

			mainConfig.pidConfig[YAW].kd   = 700.00;
			mainConfig.pidConfig[ROLL].kd  = 700.00;
			mainConfig.pidConfig[PITCH].kd = 800.00;

			mainConfig.pidConfig[YAW].ga   = 0.00;
			mainConfig.pidConfig[ROLL].ga  = 0.00;
			mainConfig.pidConfig[PITCH].ga = 0.00;

			mainConfig.pidConfig[YAW].wc   = 0;
			mainConfig.pidConfig[ROLL].wc  = 0;
			mainConfig.pidConfig[PITCH].wc = 0;

			mainConfig.filterConfig[YAW].gyro.r   = 88.000;
			mainConfig.filterConfig[ROLL].gyro.r  = 88.000;
			mainConfig.filterConfig[PITCH].gyro.r = 88.000;

			mainConfig.filterConfig[YAW].gyro.q   = 30.00;
			mainConfig.filterConfig[ROLL].gyro.q  = 30.00;
			mainConfig.filterConfig[PITCH].gyro.q = 30.00;

			mainConfig.filterConfig[YAW].gyro.p   = 0.000;
			mainConfig.filterConfig[ROLL].gyro.p  = 0.000;
			mainConfig.filterConfig[PITCH].gyro.p = 0.000;

			mainConfig.filterConfig[YAW].kd.r   = 90.0;
			mainConfig.filterConfig[ROLL].kd.r  = 90.0;
			mainConfig.filterConfig[PITCH].kd.r = 90.0;

			mainConfig.gyroConfig.filterTypeGyro = 0;
			mainConfig.gyroConfig.filterTypeKd   = 2;

			resetBoard = 1;
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "pidsdefaultsaving\n", sizeof("pidsdefaultsaving\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);

		}
	else if (!strcmp("pidsxdc", inString))
		{
			mainConfig.pidConfig[YAW].kp   = 345.00;
			mainConfig.pidConfig[ROLL].kp  = 140.00;
			mainConfig.pidConfig[PITCH].kp = 175.00;

			mainConfig.pidConfig[YAW].ki   = 475.00;
			mainConfig.pidConfig[ROLL].ki  = 370.00;
			mainConfig.pidConfig[PITCH].ki = 435.00;

			mainConfig.pidConfig[YAW].kd   = 2100.00;
			mainConfig.pidConfig[ROLL].kd  = 800.00;
			mainConfig.pidConfig[PITCH].kd = 1000.00;

			mainConfig.pidConfig[YAW].ga   = 0.00;
			mainConfig.pidConfig[ROLL].ga  = 0.00;
			mainConfig.pidConfig[PITCH].ga = 0.00;

			mainConfig.pidConfig[YAW].wc   = 0;
			mainConfig.pidConfig[ROLL].wc  = 0;
			mainConfig.pidConfig[PITCH].wc = 0;

			mainConfig.filterConfig[YAW].gyro.r   = 150.00;
			mainConfig.filterConfig[ROLL].gyro.r  = 100.00;
			mainConfig.filterConfig[PITCH].gyro.r = 100.00;

			mainConfig.filterConfig[YAW].gyro.q   = 0.010;
			mainConfig.filterConfig[ROLL].gyro.q  = 0.010;
			mainConfig.filterConfig[PITCH].gyro.q = 0.010;

			mainConfig.filterConfig[YAW].gyro.p   = 0.150;
			mainConfig.filterConfig[ROLL].gyro.p  = 0.150;
			mainConfig.filterConfig[PITCH].gyro.p = 0.150;

			mainConfig.filterConfig[YAW].kd.r     = 85.0;
			mainConfig.filterConfig[ROLL].kd.r    = 90.0;
			mainConfig.filterConfig[PITCH].kd.r   = 90.0;

			mainConfig.gyroConfig.filterTypeGyro   = 0;
			mainConfig.gyroConfig.filterTypeKd     = 2;

			resetBoard = 1;
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "pidsxdcsaving\n", sizeof("pidsxdcsaving\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("pidsrs2k", inString))
		{

			mainConfig.pidConfig[YAW].kp   = 250.00;
			mainConfig.pidConfig[ROLL].kp  = 150.00;
			mainConfig.pidConfig[PITCH].kp = 200.00;

			mainConfig.pidConfig[YAW].ki   = 600.00;
			mainConfig.pidConfig[ROLL].ki  = 500.00;
			mainConfig.pidConfig[PITCH].ki = 550.00;

			mainConfig.pidConfig[YAW].kd   = 700.00;
			mainConfig.pidConfig[ROLL].kd  = 700.00;
			mainConfig.pidConfig[PITCH].kd = 800.00;

			mainConfig.pidConfig[YAW].ga   = 0.00;
			mainConfig.pidConfig[ROLL].ga  = 0.00;
			mainConfig.pidConfig[PITCH].ga = 0.00;

			mainConfig.pidConfig[YAW].wc   = 0;
			mainConfig.pidConfig[ROLL].wc  = 0;
			mainConfig.pidConfig[PITCH].wc = 0;

			mainConfig.filterConfig[YAW].gyro.r   = 88.000;
			mainConfig.filterConfig[ROLL].gyro.r  = 88.000;
			mainConfig.filterConfig[PITCH].gyro.r = 88.000;

			mainConfig.filterConfig[YAW].gyro.q   = 30.00;
			mainConfig.filterConfig[ROLL].gyro.q  = 30.00;
			mainConfig.filterConfig[PITCH].gyro.q = 30.00;

			mainConfig.filterConfig[YAW].gyro.p   = 0.000;
			mainConfig.filterConfig[ROLL].gyro.p  = 0.000;
			mainConfig.filterConfig[PITCH].gyro.p = 0.000;

			mainConfig.filterConfig[YAW].kd.r   = 90.0;
			mainConfig.filterConfig[ROLL].kd.r  = 90.0;
			mainConfig.filterConfig[PITCH].kd.r = 90.0;

			mainConfig.gyroConfig.filterTypeGyro  = 0;
			mainConfig.gyroConfig.filterTypeKd    = 2;

			resetBoard = 1;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "pidsrs2ksaving\n", sizeof("pidsrs2ksaving\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("pidsnoise", inString))
		{
			mainConfig.pidConfig[YAW].kp   = 345.00;
			mainConfig.pidConfig[ROLL].kp  = 140.00;
			mainConfig.pidConfig[PITCH].kp = 175.00;

			mainConfig.pidConfig[YAW].ki   = 475.00;
			mainConfig.pidConfig[ROLL].ki  = 370.00;
			mainConfig.pidConfig[PITCH].ki = 435.00;

			mainConfig.pidConfig[YAW].kd   = 2100.00;
			mainConfig.pidConfig[ROLL].kd  = 800.00;
			mainConfig.pidConfig[PITCH].kd = 1000.00;

			mainConfig.pidConfig[YAW].ga   = 3.00;
			mainConfig.pidConfig[ROLL].ga  = 3.00;
			mainConfig.pidConfig[PITCH].ga = 3.00;

			mainConfig.pidConfig[YAW].wc   = 8;
			mainConfig.pidConfig[ROLL].wc  = 8;
			mainConfig.pidConfig[PITCH].wc = 8;

			mainConfig.filterConfig[YAW].gyro.r   = 180.00;
			mainConfig.filterConfig[ROLL].gyro.r  = 180.00;
			mainConfig.filterConfig[PITCH].gyro.r = 180.00;

			mainConfig.filterConfig[YAW].gyro.q   = 0.100;
			mainConfig.filterConfig[ROLL].gyro.q  = 0.100;
			mainConfig.filterConfig[PITCH].gyro.q = 0.100;

			mainConfig.filterConfig[YAW].gyro.p   = 0.150;
			mainConfig.filterConfig[ROLL].gyro.p  = 0.150;
			mainConfig.filterConfig[PITCH].gyro.p = 0.150;

			mainConfig.filterConfig[YAW].kd.r     = 80.0;
			mainConfig.filterConfig[ROLL].kd.r    = 80.0;
			mainConfig.filterConfig[PITCH].kd.r   = 80.0;

			mainConfig.gyroConfig.filterTypeGyro   = 0;
			mainConfig.gyroConfig.filterTypeKd     = 2;

			resetBoard = 1;
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "pidsrs2ksaving\n", sizeof("pidsrs2ksaving\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("pidstaja", inString))
		{
			mainConfig.pidConfig[YAW].kp = 250.00;
			mainConfig.pidConfig[ROLL].kp = 180.00;
			mainConfig.pidConfig[PITCH].kp = 240.00;

			mainConfig.pidConfig[YAW].ki = 1700.00;
			mainConfig.pidConfig[ROLL].ki = 1000.00;
			mainConfig.pidConfig[PITCH].ki = 1400.00;

			mainConfig.pidConfig[YAW].kd = 1500.00;
			mainConfig.pidConfig[ROLL].kd = 1800.00;
			mainConfig.pidConfig[PITCH].kd = 2500.00;

			mainConfig.pidConfig[YAW].ga = 2.00;
			mainConfig.pidConfig[ROLL].ga = 2.00;
			mainConfig.pidConfig[PITCH].ga = 2.00;

			mainConfig.pidConfig[YAW].wc = 2.00;
			mainConfig.pidConfig[ROLL].wc = 8.00;
			mainConfig.pidConfig[PITCH].wc = 8.00;

			mainConfig.filterConfig[YAW].gyro.r = 130.00;
			mainConfig.filterConfig[ROLL].gyro.r = 130.00;
			mainConfig.filterConfig[PITCH].gyro.r = 130.00;

			mainConfig.filterConfig[YAW].gyro.q = 0.010;
			mainConfig.filterConfig[ROLL].gyro.q = 0.015;
			mainConfig.filterConfig[PITCH].gyro.q = 0.015;

			mainConfig.filterConfig[YAW].gyro.p = 0.100;
			mainConfig.filterConfig[ROLL].gyro.p = 0.100;
			mainConfig.filterConfig[PITCH].gyro.p = 0.100;

			mainConfig.filterConfig[YAW].kd.r = 25.0;
			mainConfig.filterConfig[ROLL].kd.r = 40.0;
			mainConfig.filterConfig[PITCH].kd.r = 40.0;

			mainConfig.gyroConfig.filterTypeGyro = 0;
			mainConfig.gyroConfig.filterTypeKd = 1;

			resetBoard = 1;
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "pidstajasaving\n", sizeof("pidstajasaving\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);

		}
	else if (!strcmp("pidspg", inString))
		{
			mainConfig.pidConfig[YAW].kp = 500.00;
			mainConfig.pidConfig[ROLL].kp = 340.00;
			mainConfig.pidConfig[PITCH].kp = 380.00;

			mainConfig.pidConfig[YAW].ki = 410.00;
			mainConfig.pidConfig[ROLL].ki = 330.00;
			mainConfig.pidConfig[PITCH].ki = 365.00;

			mainConfig.pidConfig[YAW].kd = 300.00;
			mainConfig.pidConfig[ROLL].kd = 580.00;
			mainConfig.pidConfig[PITCH].kd = 640.00;

			mainConfig.pidConfig[YAW].ga = 3.00;
			mainConfig.pidConfig[ROLL].ga = 3.00;
			mainConfig.pidConfig[PITCH].ga = 3.00;

			mainConfig.pidConfig[YAW].wc = 0;
			mainConfig.pidConfig[ROLL].wc = 0;
			mainConfig.pidConfig[PITCH].wc = 0;

			mainConfig.filterConfig[YAW].gyro.r = 150.00;
			mainConfig.filterConfig[ROLL].gyro.r = 150.00;
			mainConfig.filterConfig[PITCH].gyro.r = 150.00;

			mainConfig.filterConfig[YAW].gyro.q = 0.015;
			mainConfig.filterConfig[ROLL].gyro.q = 0.015;
			mainConfig.filterConfig[PITCH].gyro.q = 0.015;

			mainConfig.filterConfig[YAW].gyro.p = 0.005;
			mainConfig.filterConfig[ROLL].gyro.p = 0.005;
			mainConfig.filterConfig[PITCH].gyro.p = 0.005;

			mainConfig.filterConfig[YAW].kd.r = 100.0;
			mainConfig.filterConfig[ROLL].kd.r = 100.0;
			mainConfig.filterConfig[PITCH].kd.r = 100.0;

			mainConfig.gyroConfig.filterTypeGyro = 0;
			mainConfig.gyroConfig.filterTypeKd = 2;

			resetBoard = 1;
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "pidspgsaving\n", sizeof("pidspgsaving\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("pidsabba", inString))
		{
			mainConfig.pidConfig[YAW].kp = 420.00;
			mainConfig.pidConfig[ROLL].kp = 280.00;
			mainConfig.pidConfig[PITCH].kp = 300.00;

			mainConfig.pidConfig[YAW].ki = 410.00;
			mainConfig.pidConfig[ROLL].ki = 330.00;
			mainConfig.pidConfig[PITCH].ki = 365.00;

			mainConfig.pidConfig[YAW].kd = 1200.00;
			mainConfig.pidConfig[ROLL].kd = 3800.00;
			mainConfig.pidConfig[PITCH].kd = 4000.00;

			mainConfig.pidConfig[YAW].ga = 3.00;
			mainConfig.pidConfig[ROLL].ga = 3.00;
			mainConfig.pidConfig[PITCH].ga = 3.00;

			mainConfig.pidConfig[YAW].wc = 4.00;
			mainConfig.pidConfig[ROLL].wc = 4.00;
			mainConfig.pidConfig[PITCH].wc = 4.00;

			mainConfig.filterConfig[YAW].gyro.r = 180.00;
			mainConfig.filterConfig[ROLL].gyro.r = 180.00;
			mainConfig.filterConfig[PITCH].gyro.r = 180.00;

			mainConfig.filterConfig[YAW].gyro.q = 0.1000;
			mainConfig.filterConfig[ROLL].gyro.q = 0.1000;
			mainConfig.filterConfig[PITCH].gyro.q = 0.1000;

			mainConfig.filterConfig[YAW].gyro.p = 0.1500;
			mainConfig.filterConfig[ROLL].gyro.p = 0.1500;
			mainConfig.filterConfig[PITCH].gyro.p = 0.1500;

			mainConfig.filterConfig[YAW].kd.r = 75.0;
			mainConfig.filterConfig[ROLL].kd.r = 90.0;
			mainConfig.filterConfig[PITCH].kd.r = 90.0;

			mainConfig.gyroConfig.filterTypeGyro = 0;
			mainConfig.gyroConfig.filterTypeKd = 2;

			resetBoard = 1;
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "pidsabbasaving\n", sizeof("pidsabbasaving\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);

		}
	else if (!strcmp("spek_t1", inString) || !strcmp("spek_t3", inString) || !strcmp("spek_t4", inString) || !strcmp("spek_r1", inString) || !strcmp("spek_r3", inString) || !strcmp("spek_r4", inString))
		{
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
			mainConfig.rcControlsConfig.minRc[AUX2]          = 1706;
			mainConfig.rcControlsConfig.minRc[AUX3]          = 342;
			mainConfig.rcControlsConfig.minRc[AUX4]          = 0;

			mainConfig.rcControlsConfig.maxRc[PITCH]         = 2025;
			mainConfig.rcControlsConfig.maxRc[ROLL]          = 2025;
			mainConfig.rcControlsConfig.maxRc[YAW]           = 2025;
			mainConfig.rcControlsConfig.maxRc[THROTTLE]      = 2025;
			mainConfig.rcControlsConfig.maxRc[AUX1]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX2]          = 342;
			mainConfig.rcControlsConfig.maxRc[AUX3]          = 1706;
			mainConfig.rcControlsConfig.maxRc[AUX4]          = 1000000;

			mainConfig.rcControlsConfig.channelMap[PITCH]    = 2;
			mainConfig.rcControlsConfig.channelMap[ROLL]     = 1;
			mainConfig.rcControlsConfig.channelMap[YAW]      = 3;
			mainConfig.rcControlsConfig.channelMap[THROTTLE] = 0;
			mainConfig.rcControlsConfig.channelMap[AUX1]     = 4;
			mainConfig.rcControlsConfig.channelMap[AUX2]     = 5;
			mainConfig.rcControlsConfig.channelMap[AUX3]     = 6;
			mainConfig.rcControlsConfig.channelMap[AUX4]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX5]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX6]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX7]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX8]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX9]     = 100;
			mainConfig.rcControlsConfig.channelMap[AUX10]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX11]    = 100;
			mainConfig.rcControlsConfig.channelMap[AUX12]    = 100; //junk channel

			mainConfig.rcControlsConfig.rcCalibrated         = 1;

			if(!strcmp("spek_t1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SPEKTRUM_T; //this is used by serial.c
			}
			if(!strcmp("spek_t3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SPEKTRUM_T; //this is used by serial.c
			}
			if(!strcmp("spek_t4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SPEKTRUM_T; //this is used by serial.c
			}
			if(!strcmp("spek_r1", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART1;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SPEKTRUM_R; //this is used by serial.c
			}
			if(!strcmp("spek_r3", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART3;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SPEKTRUM_R; //this is used by serial.c
			}
			if(!strcmp("spek_r4", inString)) {
				mainConfig.rcControlsConfig.rxUsart          = ENUM_USART4;
				mainConfig.rcControlsConfig.rxProtcol        = USING_SPEKTRUM_R; //this is used by serial.c
			}

			resetBoard = 1;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "settingdefaultstospektrumsupport\n", sizeof("settingdefaultstospektrumsupport\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("dump", inString))
		{
			uint32_t argsOutputted = 0;

			args = StripSpaces(args);

			if ( (!strcmp("back", args)) ) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "#dumpstarted\n", sizeof("#dumpstarted\n"));
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, FULL_VERSION_STRING "\0", sizeof(FULL_VERSION_STRING "\0"));
				RfCustomReply(rf_custom_out_buffer);
			}
			else
			{
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "dumpstarted\n", sizeof("dumpstarted\n"));
				RfCustomReply(rf_custom_out_buffer);
			}



			if ( (!strcmp("", args)) || (!strcmp("all", args)) ) {
				for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
				{
					OutputVar(x);
					argsOutputted++;
				}
			} else if ( (!strcmp("back", args)) ) {
				for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
				{
					OutputVarSet(x);
					argsOutputted++;
				}
			} else {
				for (x=0;x<(sizeof(valueTable)/sizeof(config_variables_rec));x++)
				{
					if (!strcmp(valueTable[x].group, args)) {
						OutputVar(x);
						argsOutputted++;
					}
				}
			}

			//char argString[12];
			//ftoa(pitchAttitude, pitchString);
			//StripSpaces(yawString);

			if (argsOutputted == 0) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "noargumentsfoundforargument:%s\n", args);
				RfCustomReply(rf_custom_out_buffer);
			}

			//output flash here:
			if ( (!strcmp("", args)) || (!strcmp("all", args)) || (!strcmp("flash", args)) ) {
				if ( (!strcmp("back", args)) )
				{

				}
				else
				{
					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "dlflused=%u\n", (unsigned int)(flashInfo.currentWriteAddress));
					RfCustomReply(rf_custom_out_buffer);
					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "dlfltotal=%u\n", (unsigned int)(flashInfo.totalSize));
					RfCustomReply(rf_custom_out_buffer);
					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "dlflsectors=%u\n", (unsigned int)(flashInfo.flashSectors));
					RfCustomReply(rf_custom_out_buffer);
				}
			}

			if ( (!strcmp("back", args)) )
			{
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "#dumpcomplete\n", sizeof("#dumpcomplete\n"));
				RfCustomReply(rf_custom_out_buffer);
			}
			else
			{
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "dumpcomplete\n", sizeof("dumpcomplete\n"));
				RfCustomReply(rf_custom_out_buffer);
			}

		}
	else if (!strcmp("eraseallflash", inString))
		{

			if (flashInfo.enabled) {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "erasingflash\n", sizeof("erasingflash\n"));
				RfCustomReply(rf_custom_out_buffer);

				if (MassEraseDataFlash(1)) {
					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					memcpy(rf_custom_out_buffer, "flasherasecomplete\n", sizeof("flasherasecomplete\n"));
					RfCustomReply(rf_custom_out_buffer);
				} else {
					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					memcpy(rf_custom_out_buffer, "flasherasefailed\n", sizeof("flasherasefailed\n"));
					RfCustomReply(rf_custom_out_buffer);
				}

			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected\n", sizeof("flashchipnotdetected\n"));
				RfCustomReply(rf_custom_out_buffer);

			}

		}
	else if (!strcmp("eraseflash", inString))
		{
			if (flashInfo.enabled) {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "erasingflash", sizeof("erasingflash"));
				RfCustomReply(rf_custom_out_buffer);

				if (((float)(flashInfo.currentWriteAddress * flashInfo.pageSize * flashInfo.pagesPerSector) / (float)flashInfo.totalSize) > 0.85) {
					if (MassEraseDataFlash(1)) {
						bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
						memcpy(rf_custom_out_buffer, "flasherasecomplete", sizeof("flasherasecomplete"));
						RfCustomReply(rf_custom_out_buffer);
					} else {
						bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
						memcpy(rf_custom_out_buffer, "flasherasefailed", sizeof("flasherasefailed"));
						RfCustomReply(rf_custom_out_buffer);
					}
				} else {
					if (MassEraseDataFlashByPage(1)) {
						bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
						memcpy(rf_custom_out_buffer, "flasherasecomplete", sizeof("flasherasecomplete"));
						RfCustomReply(rf_custom_out_buffer);
					}
				}

			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("startlog", inString))
		{
			if (flashInfo.enabled) {
				curvedRcCommandF[AUX2] = -0.6;
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "loggingstarted", sizeof("loggingstarted"));
				RfCustomReply(rf_custom_out_buffer);

			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("endlog", inString))
		{
			if (flashInfo.enabled) {
				curvedRcCommandF[AUX2] = 0.6;
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "loggingended", sizeof("loggingended"));
				RfCustomReply(rf_custom_out_buffer);

			} else {
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);
			}
		}
	else if (!strcmp("dlflsize", inString))
		{


			if (flashInfo.enabled) {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)(flashInfo.currentWriteAddress));
				RfCustomReply(rf_custom_out_buffer);

			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)0);
				RfCustomReply(rf_custom_out_buffer);

			}
		}
	else if (!strcmp("dlfltotal", inString))
		{


			if (flashInfo.enabled) {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)(flashInfo.totalSize));
				RfCustomReply(rf_custom_out_buffer);

			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)0);
				RfCustomReply(rf_custom_out_buffer);

			}
		}
	else if (!strcmp("dlflsectors", inString))
		{


			if (flashInfo.enabled) {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)(flashInfo.flashSectors));
				RfCustomReply(rf_custom_out_buffer);

			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)0);
				RfCustomReply(rf_custom_out_buffer);

			}
		}
	else if ( (!strcmp("downloadflightlog", inString)) || (!strcmp("dlb", inString)) || (!strcmp("dlfl", inString)) || (!strcmp("dlflslow", inString)) )
		{
			int base64Encode = 0;

			if (!strcmp("dlflslow", inString))
				DelayMs(1000);

			args = StripSpaces(args);
			if (!strcmp("b64", args))
				base64Encode = 1;

			if (!strcmp("dlb", inString))
				base64Encode = 1;

			if (flashInfo.enabled) {

				if(flashInfo.currentWriteAddress < 256) {

					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					memcpy(rf_custom_out_buffer, "flightlogempty", sizeof("flightlogempty"));
					RfCustomReply(rf_custom_out_buffer);

				} else {

					uint8_t dataArray[45];
					uint32_t smallerPointer = 0;
					uint32_t pagesToSend = ((flashInfo.currentWriteAddress) / flashInfo.pageSize);

					for (uint32_t y = 0;y<pagesToSend;y++) {

						if ( M25p16ReadPage( (y * flashInfo.pageSize), flashInfo.buffer[0].txBuffer, flashInfo.buffer[0].rxBuffer) ) {

							for (uint32_t x=0;x<256;x++) {

								if (base64Encode) {
									dataArray[smallerPointer++] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
									if (smallerPointer > 44) {

										//base64 encode
										for (smallerPointer = 0; smallerPointer < 15; smallerPointer++) {
											EncodeBlock( (unsigned char *)( dataArray + (smallerPointer*3) ), (unsigned char *)( rf_custom_out_buffer + (smallerPointer*4) ), 4);
										}

										RfCustomReply(rf_custom_out_buffer);
										smallerPointer = 0;
										bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
										bzero(dataArray,sizeof(dataArray));
									}

								} else {

									rf_custom_out_buffer[smallerPointer++] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
									if (smallerPointer > 62) {
										RfCustomReply(rf_custom_out_buffer);
										smallerPointer = 0;
										bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
										if (!strcmp("dlflslow", inString))
											DelayMs(6);
									}

								}

							}

						} else {

							bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
							memcpy(rf_custom_out_buffer, "flashreadfailed", sizeof("flashreadfailed"));
							RfCustomReply(rf_custom_out_buffer);

						}

					}

				}
			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "downloadflightlogstarted", sizeof("downloadflightlogstarted"));
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "%u", (unsigned int)378);
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=45;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=11;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=22;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=55;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=33;
				RfCustomReply(rf_custom_out_buffer);
				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				for (uint32_t fft=0;fft<RF_BUFFER_SIZE;fft++)
					rf_custom_out_buffer[fft]=21;
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "downloadflightlogfinished", sizeof("downloadflightlogfinished"));
				RfCustomReply(rf_custom_out_buffer);

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);

			}

		}
	else if (!strcmp("readflash", inString))
		{
			uint32_t smallerPointer;
			if (flashInfo.enabled) {

				args = StripSpaces(args);

				if ( M25p16ReadPage( atoi(args), flashInfo.buffer[0].txBuffer, flashInfo.buffer[0].rxBuffer) ) {

					//for (uint32_t x=0;x<256;x++) {
					for (uint32_t x=0;x<RF_BUFFER_SIZE;x++) {

						rf_custom_out_buffer[smallerPointer++] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
						//rf_custom_out_buffer[smallerPointer++] = flashInfo.buffer[0].rxBuffer[FLASH_CHIP_BUFFER_READ_DATA_START+x];
						//if (smallerPointer > 62) {
						//	RfCustomReply(rf_custom_out_buffer);
						//	smallerPointer = 0;
						//	bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
						//}

					}
					RfCustomReply(rf_custom_out_buffer);
					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));

				} else {

					bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
					memcpy(rf_custom_out_buffer, "flashreadfailed", sizeof("flashreadfailed"));
					RfCustomReply(rf_custom_out_buffer);

				}

			} else {

				bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
				memcpy(rf_custom_out_buffer, "flashchipnotdetected", sizeof("flashchipnotdetected"));
				RfCustomReply(rf_custom_out_buffer);

			}

		}
	else if (!strcmp("version", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, FULL_VERSION_STRING "\0", sizeof(FULL_VERSION_STRING "\0"));
			RfCustomReply(rf_custom_out_buffer);
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
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("reboot", inString) || !strcmp("reset", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebooting\n", sizeof("rebooting\n"));
			RfCustomReply(rf_custom_out_buffer);
			SystemReset();
		}
	else if (!strcmp("resetdfu", inString)  || !strcmp("rebootdfu", inString))
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebootingdfu\n", sizeof("rebootingdfu\n"));
			RfCustomReply(rf_custom_out_buffer);
			SystemResetToDfuBootloader();
		}
	else if (!strcmp("resetconfig", inString))
		{
			resetBoard = 1;

			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "resettingconfig\n", sizeof("resettingconfig\n"));
			RfCustomReply(rf_custom_out_buffer);
			GenerateConfig();
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "configreset\n", sizeof("configreset\n"));
			RfCustomReply(rf_custom_out_buffer);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "saving\n", sizeof("saving\n"));
			RfCustomReply(rf_custom_out_buffer);
			SaveConfig(ADDRESS_CONFIG_START);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "savecomplete\n", sizeof("savecomplete\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("binds", inString))
		{
			sendSpektrumBind();
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "sendingserialbind\n", sizeof("sendingserialbind\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if ( (!strcmp("bind9", inString)) || (!strcmp("bind", inString)) )
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM9);
			rtc_write_backup_reg(FC_STATUS_REG,BOOT_TO_SPEKTRUM9);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "binding9\n", sizeof("binding9\n"));
			RfCustomReply(rf_custom_out_buffer);
		}
	else if (!strcmp("bind5", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM5);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "binding5\n", sizeof("binding5\n"));
			RfCustomReply(rf_custom_out_buffer);
			DelayMs(100);
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_SPEKTRUM5);
			DelayMs(100);
		}
	else if (!strcmp("rebootrfbl", inString) || !strcmp("resetrfbl", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RFBL_COMMAND);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebootrfbl\n", sizeof("rebootrfbl\n"));
			RfCustomReply(rf_custom_out_buffer);
			DelayMs(100);
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RFBL_COMMAND);
			DelayMs(100);
			SystemReset();
		}
	else if (!strcmp("rebootrecovery", inString) || !strcmp("resetrecovery", inString))
		{
			rtc_write_backup_reg(RFBL_BKR_BOOT_DIRECTION_REG,BOOT_TO_RECOVERY_COMMAND);
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			memcpy(rf_custom_out_buffer, "rebootrecovery\n", sizeof("rebootrecovery\n"));
			RfCustomReply(rf_custom_out_buffer);
			DelayMs(100);
			SystemReset();
		}
	else if (!strcmp("1wire", inString))
		{
			args = StripSpaces(args);
			OneWire(args);
		}
	else
		{
			bzero(rf_custom_out_buffer,sizeof(rf_custom_out_buffer));
			snprintf(rf_custom_out_buffer, RF_BUFFER_SIZE, "unknowncommand:%s\n", args);
			RfCustomReply(rf_custom_out_buffer);
		}

}





