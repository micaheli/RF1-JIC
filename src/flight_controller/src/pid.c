#include "includes.h"

paf_state kdFilterState[AXIS_NUMBER];
float currentKdFilterConfig[AXIS_NUMBER];
float kdRingBuffer[AXIS_NUMBER][KD_RING_BUFFER_SIZE];
float kdRingBufferSum[AXIS_NUMBER];
uint32_t kdRingBufferPoint[AXIS_NUMBER];
float kdDelta[AXIS_NUMBER];

//1000000 114
pid_terms  pidsUsed[AXIS_NUMBER];

float dT; //8KHz
uint32_t uhohNumber = 4000;
uint32_t uhOhRecover = 0;
volatile uint32_t khzDivider = 0;

/*
 *     LOOP_L1,
    LOOP_M1,
    LOOP_M2,
    LOOP_M4,
    LOOP_M8,
    LOOP_H1,
    LOOP_H2,
    LOOP_H4,
    LOOP_H8,
    LOOP_H16,
    LOOP_H32,
    LOOP_UH1,
    LOOP_UH2,
    LOOP_UH4,
    LOOP_UH8,
    LOOP_UH16,
    LOOP_UH32,
 */
void InitPid (void) {

	bzero(kdDelta, sizeof(kdDelta));
	bzero(kdRingBuffer, sizeof(kdRingBuffer));
	bzero(kdRingBufferSum, sizeof(kdRingBufferSum));
	bzero(kdRingBufferPoint, sizeof(kdRingBufferPoint));
	InlineInitPidFilters();

	uhOhRecover = 0; //unset recover mode

	switch (mainConfig.gyroConfig.loopCtrl) {
		case LOOP_UH32:
		case LOOP_H32:
			dT = 0.00003125;
			uhohNumber = 16000;
			khzDivider = 32;
			break;
		case LOOP_UH16:
		case LOOP_H16:
			dT = 0.00006250;
			uhohNumber = 8000;
			khzDivider = 16;
			break;
		case LOOP_UH8:
		case LOOP_H8:
		case LOOP_M8:
			dT = 0.00012500;
			uhohNumber = 4000;
			khzDivider = 8;
			break;
		case LOOP_UH4:
		case LOOP_H4:
		case LOOP_M4:
			dT = 0.00025000;
			uhohNumber = 2000;
			khzDivider = 4;
			break;
		case LOOP_UH2:
		case LOOP_H2:
		case LOOP_M2:
			dT = 0.00050000;
			uhohNumber = 1000;
			khzDivider = 2;
			break;
		case LOOP_UH1:
		case LOOP_H1:
		case LOOP_M1:
		case LOOP_L1:
		default:
			dT = 0.00100000;
			uhohNumber = 500;
			khzDivider = 1;
			break;
	}

	//0.00   (x / 10000) * 1 = .00140000
	//0.00   (x / 10000) * 0.00003125 = .00140000
	//0.00   (x * 100) * 0.00003125 = .00000280

	//140
	//140
	//200

	pidsUsed[0].kp = mainConfig.pidConfig[0].kp / 100000;
	pidsUsed[0].ki = (mainConfig.pidConfig[0].ki / 50000) * dT;
	pidsUsed[0].kd = (mainConfig.pidConfig[0].kd * 12 )  * dT;
	pidsUsed[0].wc = mainConfig.pidConfig[0].wc;

	pidsUsed[1].kp = mainConfig.pidConfig[1].kp / 100000;
	pidsUsed[1].ki = (mainConfig.pidConfig[1].ki / 50000) * dT;
	pidsUsed[1].kd = (mainConfig.pidConfig[1].kd * 12 )  * dT;
	pidsUsed[1].wc = mainConfig.pidConfig[1].wc;

	pidsUsed[2].kp = mainConfig.pidConfig[2].kp / 100000;
	pidsUsed[2].ki = (mainConfig.pidConfig[2].ki / 50000) * dT;
	pidsUsed[2].kd = (mainConfig.pidConfig[2].kd * 12 )  * dT;
	pidsUsed[2].wc = mainConfig.pidConfig[2].wc;

}

inline void InlineInitPidFilters (void) {

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {

		kdFilterState[axis] = InitPaf( mainConfig.filterConfig[axis].kd.q, mainConfig.filterConfig[axis].kd.r, mainConfig.filterConfig[axis].kd.p, kdDelta[axis]);

		currentKdFilterConfig[axis] = mainConfig.filterConfig[axis].kd.r;

	}

}

inline void InlinePidController (float filteredGyroData[], float flightSetPoints[], pid_output flightPids[], float actuatorRange, pid_terms pidConfig[]) {

	int32_t axis;

	float pidError;
	static float lastfilteredGyroData[AXIS_NUMBER];
	static float usedFlightSetPoints[AXIS_NUMBER];
	static float kiErrorLimit[AXIS_NUMBER];

	(void)(pidConfig);

	//set point limiter.
	if ( actuatorRange >= 0.90 ) {
		//we don't change the setpoint when actuators are maxed, unless setpoint is shrinking
		for (axis = 2; axis >= 0; --axis) {
			if ( abs(usedFlightSetPoints[axis]) > abs(flightSetPoints[axis]) ) {
				usedFlightSetPoints[axis] = flightSetPoints[axis];
			}
		}
	} else if ( actuatorRange >= 0.70 ) {
		//if actuator is near max, we limit the change of the setpoint unless the setpoint is shrinking
		for (axis = 2; axis >= 0; --axis) {
			if ( abs(usedFlightSetPoints[axis]) > abs(flightSetPoints[axis]) ) {
				usedFlightSetPoints[axis] = flightSetPoints[axis];
			} else {
				usedFlightSetPoints[axis] += ( (flightSetPoints[axis]-usedFlightSetPoints[axis]) * 0.5 );
			}
		}
	} else {
		//else we set the full setpoint
		usedFlightSetPoints[0] = flightSetPoints[0];
		usedFlightSetPoints[1] = flightSetPoints[1];
		usedFlightSetPoints[2] = flightSetPoints[2];
	}

	//usedFlightSetPoints[0] = flightSetPoints[0];
	//usedFlightSetPoints[1] = flightSetPoints[1];
	//usedFlightSetPoints[2] = flightSetPoints[2];

	for (axis = 2; axis >= 0; --axis) {

		pidError = usedFlightSetPoints[axis] - filteredGyroData[axis];

	    if ( SpinStopper(axis, pidError) ) {
		//if ( 0 ) {

	    	flightPids[axis].kp = 0;
	    	flightPids[axis].ki = 0;
	    	flightPids[axis].kd = 0;

	    } else {

			// calculate Kp
			flightPids[axis].kp = (pidError * pidsUsed[axis].kp);

			// calculate Ki
			if ( fullKiLatched ) {

				flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidError * pidsUsed[axis].ki, -0.212121f, 0.212121f); //prevent insane windup

				if ( actuatorRange > .95 ) { //actuator maxed out, don't allow Ki to increase to prevent windup from maxed actuators
					flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki, -kiErrorLimit[axis], kiErrorLimit[axis]);
				} else {
					kiErrorLimit[axis] = ABS(kiErrorLimit[axis]);
				}

			} else {

				flightPids[axis].ki = InlineConstrainf(flightPids[axis].ki + pidError * pidsUsed[axis].ki, -0.02121f, 0.02121f); //limit Ki when fullKiLatched is false

			}

			// calculate Kd ////////////////////////// V
			kdDelta[axis] = -(filteredGyroData[axis] - lastfilteredGyroData[axis]);
			lastfilteredGyroData[axis] = filteredGyroData[axis];

			//updated Kd filter
			if (mainConfig.filterConfig[axis].kd.r) {
				PafUpdate(&kdFilterState[axis], kdDelta[axis]);
				kdDelta[axis] = kdFilterState[axis].x;
			}

			InlineUpdateWitchcraft(pidsUsed);

			flightPids[axis].kd = InlineConstrainf(kdDelta[axis] * pidsUsed[axis].kd, -0.3510f, 0.3510f);
			// calculate Kd ////////////////////////// ^

	    }

	}



}

inline uint32_t SpinStopper(int32_t axis, float pidError) {

    static uint32_t countErrorUhoh[AXIS_NUMBER]  = {0, 0, 0};
    static uint32_t uhOhRecoverCounter = 0;

	if (!uhOhRecover) {
		uhOhRecoverCounter = 0;
		if (ABS(pidError) > 1000) {
			countErrorUhoh[axis]++;
		} else {
			countErrorUhoh[axis] = 0;
		}
		if (countErrorUhoh[axis] > uhohNumber ) {
			uhOhRecover = 1;
		}
	} else {
		uhOhRecoverCounter++;
	}
	if (uhOhRecoverCounter > uhohNumber) {
		uhOhRecover = 0;
		uhOhRecoverCounter = 0;
	}
	if (uhOhRecoverCounter) {
		return (1);
	}
	return (0);
}

inline void InlineUpdateWitchcraft(pid_terms pidConfig[]) {

	int32_t axis;

	for (axis = 2; axis >= 0; --axis) {
		if (pidConfig[axis].wc) {
			kdRingBuffer[axis][kdRingBufferPoint[axis]++] = kdDelta[axis];
			kdRingBufferSum[axis] += kdDelta[axis];

			if (kdRingBufferPoint[axis] == pidConfig[axis].wc)
				kdRingBufferPoint[axis] = 0;

			kdRingBufferSum[axis] -= kdRingBuffer[axis][kdRingBufferPoint[axis]];
			kdDelta[axis] = (float)(kdRingBufferSum[axis] / (float) (pidConfig[axis].wc));
		}
	}

}

/*
#define KD_RING_BUFFER_SIZE 256

float kd_ring_buffer_p[KD_RING_BUFFER_SIZE];
float kd_ring_buffer_p_sum = 0;
uint32_t kd_ring_buffer_p_pointer = 0;
float kd_ring_buffer_r[KD_RING_BUFFER_SIZE];
float kd_ring_buffer_r_sum = 0;
uint32_t kd_ring_buffer_r_pointer = 0;
float kd_ring_buffer_y[KD_RING_BUFFER_SIZE];
float kd_ring_buffer_y_sum = 0;
uint32_t kd_ring_buffer_y_pointer = 0;

static void pidLuxFloat(pidProfile_t *pidProfile, controlRateConfig_t *controlRateConfig,
        uint16_t max_angle_inclination, rollAndPitchTrims_t *angleTrim, rxConfig_t *rxConfig)
{
	float RateError, AngleRate, gyroRate;
	float ITerm, PTerm, DTerm;
	//float Input;
	static float lastRate[3] = { 0, 0, 0 }, lastError[3] = { 0, 0, 0 }, InputUsed[3] = { 0, 0, 0 }, LastInput[3] = { 0, 0, 0 };
	float delta;
	static float lastRcCommand[3] = { 0, 0, 0 };
	int axis;
    float horizonLevelStrength = 1;
    bool kdTypeM, highSpeedStickMovement;
    static uint32_t countErrorUhoh[3] = {0, 0, 0};
    static bool uhOhRecover = false;
    static uint32_t uhOhRecoverCounter = 0;
	static uint16_t uhohNumber = 4000;
	static uint8_t yawCounter = 0;
	static uint8_t witchcraftMultiplier = 1;
	static uint8_t usedWitchcraft = 0;

	if (targetESCwritetime < 125) {
		uhohNumber = 8000;
		witchcraftMultiplier = 3;
	}

	usedWitchcraft = (pidProfile->witchcraft * witchcraftMultiplier);

    if (IS_RC_MODE_ACTIVE(BOXBRAINDRAIN)) {
    	onlyUseErrorMethodForKd = true;
    	onlyUseMeasureMethodForKd = false;
    } else if (IS_RC_MODE_ACTIVE(BOXSKITZO)) {
		onlyUseErrorMethodForKd = false;
		onlyUseMeasureMethodForKd = true;
    } else {
    	onlyUseErrorMethodForKd = false;
    	onlyUseMeasureMethodForKd = false;
    }

    yawCounter++;

    for (axis = 0; axis < 3; axis++)  {
    	if (axis == FD_ROLL) {
        	currentLPF = pidProfile->wrkdlpf;
		} else if (axis == FD_PITCH) {
	    	currentLPF = pidProfile->wpkdlpf;
		} else if (axis == FD_YAW) {
	    	currentLPF = pidProfile->wykdlpf;
		}
		if (!deltaStateIsSet && currentLPF) {
			BiQuadNewLpf(currentLPF, &deltaBiQuadState[axis], 0);
		}
    }
    deltaStateIsSet = true;

    // ----------PID controller----------
    for (axis = 0; axis < 3; axis++) {
        // -----Get the desired angle rate depending on flight mode
        float rate = controlRateConfig->rates[axis];

			if (axis == FD_YAW) {

				if (controlRateConfig->YawAcroPlusFactor) {
					wow_factor = fabsf(rcCommandUsed[axis] / 500.0f) * (controlRateConfig->YawAcroPlusFactor / 100.0f); //0-1f
					factor = wow_factor * rcCommandUsed[axis] + rcCommandUsed[axis];
				} else {
					factor = rcCommandUsed[axis]; // 200dps to 1200dps max roll/pitch rate
				}
				AngleRate = (float)((rate) * factor) / 500.0f; // 200dps to 1200dps max roll/pitch rate
				AngleRate = constrainf(AngleRate, -1500, 1500);

				 // ACRO mode, control is GYRO based, direct sticks control is applied to rate PID
			 } else if (axis == FD_PITCH) {
    	         if (controlRateConfig->PitchAcroPlusFactor) {
            		 wow_factor = fabsf(rcCommandUsed[axis] / 500.0f) * ((float)controlRateConfig->PitchAcroPlusFactor / 100.0f); //0-1f
            		 factor = (int16_t)(wow_factor * (float)rcCommandUsed[axis]) + rcCommandUsed[axis];
            	 } else {
            		 factor = rcCommandUsed[axis];
            	 }
        	 } else {
    	         if (controlRateConfig->RollAcroPlusFactor) {
            		 wow_factor = fabsf(rcCommandUsed[axis] / 500.0f) * ((float)controlRateConfig->RollAcroPlusFactor / 100.0f); //0-1f
            		 factor = (int16_t)(wow_factor * (float)rcCommandUsed[axis]) + rcCommandUsed[axis];
            	 } else {
            		 factor = rcCommandUsed[axis];
            	 }
        	 }

    		 AngleRate = (float)((rate) * factor) / 500.0f; // 200dps to 1200dps max roll/pitch rate
    		 AngleRate = constrainf(AngleRate, -1500, 1500);

        }


	    InputUsed[axis] = AngleRate;


	    LastInput[axis] = InputUsed[axis]; //store last this input for next iteration to find InputDelta

	    //Let's try 5% stick movement (maybe we need to make this tuneable)
	    // -500 to 500 is 1000. 5% of 1000 is 50
	    if (ABS(lastRcCommand[axis] - rcCommandUsed[axis]) > 50) {
	    	highSpeedStickMovement = true;
	    } else {
	    	highSpeedStickMovement = false;
	    }

	    lastRcCommand[axis] = rcCommandUsed[axis];

	    if ( (AngleRate <= 0) && (LastInput[axis] <= 0) && (AngleRate < LastInput[axis]) ) { //input negative and getting smaller
	    	kdTypeM = true;
	    } else if ( (AngleRate > 0) && (LastInput[axis] > 0) && (AngleRate > LastInput[axis]) ) { //input positive and getting larger
	    	kdTypeM = true;
	    } else {
	    	kdTypeM = false;
	    }

        gyroRate = gyroADC[axis] * gyro.scale; // gyro output scaled to dps

		if (!uhOhRecover) {
			uhOhRecoverCounter = 0;
			if (ABS(gyroRate - AngleRate) > 1000) {
				countErrorUhoh[axis]++;
			} else {
				countErrorUhoh[axis] = 0;
			}
			if (countErrorUhoh[axis] > uhohNumber ) {
				uhOhRecover = true;
			}
		} else {
			uhOhRecoverCounter++;
		}

		if (uhOhRecoverCounter > uhohNumber) {
			uhOhRecover = false;
			uhOhRecoverCounter = 0;
		}

        // --------low-level gyro-based PID. ----------
        // Used in stand-alone mode for ACRO, controlled by higher level regulators in other modes
        // -----calculate scaled error.AngleRates
        // multiplication of rcCommandUsed corresponds to changing the sticks scaling here
        RateError = AngleRate - gyroRate;

        // -----calculate P component
        PTerm = RateError * (pidProfile->P_f[axis]/4);
        if (!FullKiLatched) { PTerm = PTerm / 2; }

	    if (axis == YAW && pidProfile->yaw_pterm_cut_hz) {
		    PTerm = filterApplyPt1(PTerm, &yawPTermState, pidProfile->yaw_pterm_cut_hz, dT);
	    }


	    if (axis == FD_PITCH) {

	        // -----calculate I component.
	        if (FullKiLatched) {
		        errorGyroIf[axis] = constrainf(errorGyroIf[axis] + RateError * dT * (pidProfile->I_f[axis] / 2)  * 10, -250.0f, 250.0f);
		        if (motorLimitReached) {
			        errorGyroIf[axis] = constrainf(errorGyroIf[axis], -errorGyroIfLimit[axis], errorGyroIfLimit[axis]);
		        } else {
			        errorGyroIfLimit[axis] = ABS(errorGyroIf[axis]);
		        }
	        } else {
	            errorGyroIf[axis] = constrainf(errorGyroIf[axis] + RateError * dT * (pidProfile->I_f[axis]/2)  * 10, -20.0f, 20.0f);
	        }

	        // limit maximum integrator value to prevent WindUp - accumulating extreme values when system is saturated.
	        // I coefficient (I8) moved before integration to make limiting independent from PID settings

	        ITerm = errorGyroIf[axis];

	    } else {

	        // -----calculate I component.
			errorGyroIf[axis] = errorGyroIf[axis] + RateError;
			if (motorLimitReached) {
				errorGyroIf[axis] = constrainf(errorGyroIf[axis], -errorGyroIfLimit[axis], errorGyroIfLimit[axis]);
			} else {
				errorGyroIfLimit[axis] = ABS(errorGyroIf[axis]);
			}

	        // limit maximum integrator value to prevent WindUp - accumulating extreme values when system is saturated.
	        // I coefficient (I8) moved before integration to make limiting independent from PID settings

	        if (FullKiLatched) {
	        	ITerm = constrainf( errorGyroIf[axis] * dT * (pidProfile->I_f[axis]/2)  * 10, -250.0f, 250.0f);
	        } else {
	            ITerm = constrainf( errorGyroIf[axis] * dT * (pidProfile->I_f[axis]/2)  * 10, -20.0f, 20.0f);
	        }

	    }


        //-----calculate D-term
	    if (onlyUseMeasureMethodForKd) {
    	    delta = -(gyroRate - lastRate[axis]);
    	    lastRate[axis] = gyroRate;
	    } else if (!kdTypeM && (ABS(rcCommandUsed[axis]) > 150) && (highSpeedStickMovement) && !!onlyUseErrorMethodForKd) {
    	    delta = -(gyroRate - lastRate[axis]);
    	    lastRate[axis] = gyroRate;
        } else {
    	    delta = (RateError) - lastError[axis];
	        lastError[axis] = (RateError);
        }

        // Correct difference by cycle time. Cycle time is jittery (can be different 2 times), so calculated difference
        // would be scaled by different dt each time. Division by dT fixes that.
	    delta *= (1.0f / dT);

		//pidProfile->witchcraft
		if (pidProfile->witchcraft) {
			if (axis == PITCH)
			{
				kd_ring_buffer_p[kd_ring_buffer_p_pointer++] = delta;
				kd_ring_buffer_p_sum += delta;

				if (kd_ring_buffer_p_pointer == usedWitchcraft)
					kd_ring_buffer_p_pointer = 0;

				kd_ring_buffer_p_sum -= kd_ring_buffer_p[kd_ring_buffer_p_pointer];
				delta = (float)(kd_ring_buffer_p_sum / (float) (usedWitchcraft));

			}
			else if (axis == ROLL)
			{
				kd_ring_buffer_r[kd_ring_buffer_r_pointer++] = delta;
				kd_ring_buffer_r_sum += delta;

				if (kd_ring_buffer_r_pointer == usedWitchcraft)
					kd_ring_buffer_r_pointer = 0;

				kd_ring_buffer_r_sum -= kd_ring_buffer_r[kd_ring_buffer_r_pointer];
				delta = (float)(kd_ring_buffer_r_sum / (float) (usedWitchcraft));
			}
			else if (axis == YAW)
			{
				kd_ring_buffer_y[kd_ring_buffer_y_pointer++] = delta;
				kd_ring_buffer_y_sum += delta;

				if (kd_ring_buffer_y_pointer == usedWitchcraft)
					kd_ring_buffer_y_pointer = 0;

				kd_ring_buffer_y_sum -= kd_ring_buffer_y[kd_ring_buffer_y_pointer];
				delta = (float)(kd_ring_buffer_y_sum / (float)(usedWitchcraft));
			}
		}

	    if (deltaStateIsSet) {
		    if (axis == YAW && pidProfile->wykdlpf) {
			    if (pidProfile->wykdlpf) delta = applyBiQuadFilter(delta, &deltaBiQuadState[axis]);
		    }
		    else if (axis == PITCH && pidProfile->wpkdlpf) {
			    if (pidProfile->wpkdlpf) delta = applyBiQuadFilter(delta, &deltaBiQuadState[axis]);
		    }
		    else if (axis == ROLL && pidProfile->wrkdlpf) {
			    if (pidProfile->wrkdlpf) delta = applyBiQuadFilter(delta, &deltaBiQuadState[axis]);
		    }
	    }

	    float D_f = pidProfile->D_f[axis];
	    //static float Kd_attenuation_break = 0.25f;
	    //if (Throttle_p < Kd_attenuation_break) {
		//    float Kd_attenuation = constrainf((Throttle_p / Kd_attenuation_break) + 0.50, 0, 1);
		//    D_f = Kd_attenuation * D_f;
	    //}

	    //if (pidProfile->dterm_lpf_hz) delta = filterApplyPt1(delta, &deltaFilterState[axis], pidProfile->dterm_lpf_hz, dT);

	    if (targetESCwritetime == 31) {
	    	D_f *= 0.33f; //1/4 dterm id 32 khz
	    }
	    DTerm = constrainf(delta * (D_f / 10), -350.0f, 350.0f);

        // -----calculate total PID output
	    if (!FullKiLatched) {
			axisPID[axis] = constrain(lrintf(PTerm + ITerm + DTerm), -200, 200);
	    } else {
		    axisPID[axis] = constrain(lrintf(PTerm + ITerm + DTerm), -1000, 1000);
	    }

	    if (uhOhRecoverCounter > 0) {
	    	axisPID[axis] = 0;
	    	PTerm = 0;
	    	ITerm = 0;
	    	DTerm = 0;
	    }

#ifdef GTUNE
        if (FLIGHT_MODE(GTUNE_MODE) && ARMING_FLAG(ARMED)) {
            calculate_Gtune(axis);
        }
#endif

        axisPID_P[axis] = PTerm;
        axisPID_I[axis] = ITerm;
        axisPID_D[axis] = DTerm;
    }
}
*/
