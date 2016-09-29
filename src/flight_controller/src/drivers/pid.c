#include "includes.h"

pid_output pids[AXIS_NUMBER] = {
	{0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f},
	{0.0f, 0.0f, 0.0f}
};

void pidController (float dpsGyroArray[], float curvedRcCommandF[], pid_output pids[], float actuatorRange) {
	dpsGyroArray = dpsGyroArray;
	curvedRcCommandF = curvedRcCommandF;
	pids = pids;
	actuatorRange = actuatorRange;
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
