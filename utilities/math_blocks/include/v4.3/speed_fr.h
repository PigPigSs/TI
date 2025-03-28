//#############################################################################
// $Copyright:
// Copyright (C) 2017-2024 Texas Instruments Incorporated - http://www.ti.com/
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//   Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
//
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the
//   documentation and/or other materials provided with the
//   distribution.
//
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

/* =================================================================================
File name:        SPEED_FR.H  
===================================================================================*/


#ifndef __SPEED_FR_H__
#define __SPEED_FR_H__

typedef struct {
       float32_t ElecTheta;  	// Input: Electrical angle (pu)
       uint32_t DirectionQep; 	// Variable: Direction of rotation (Q0) - independently with global Q
       float32_t OldElecTheta;  // History: Electrical angle at previous step (pu)
       float32_t Speed;       	// Output: Speed in per-unit  (pu)
       uint32_t BaseRpm;      	// Parameter: Base speed in rpm (Q0) - independently with global Q
       float32_t K1;         	// Parameter: Constant for differentiator (Q21) - independently with global Q
       float32_t K2;         	// Parameter: Constant for low-pass filter (pu)
       float32_t K3;         	// Parameter: Constant for low-pass filter (pu)
       int32_t SpeedRpm;      	// Output : Speed in rpm  (Q0) - independently with global Q
       float32_t Tmp;			//Variable: Temp variable
       } SPEED_MEAS_QEP;   		// Data type created

/*-----------------------------------------------------------------------------
Default initalizer for the SPEED_MEAS_QEP object.
-----------------------------------------------------------------------------*/                     
#define SPEED_MEAS_QEP_DEFAULTS   { 0, \
                                    1, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                                    0, \
                                    0  \
                               	  }	

/*------------------------------------------------------------------------------
 SPEED_FR Macro Definition
------------------------------------------------------------------------------*/

static inline float32_t runSpeedFR(SPEED_MEAS_QEP * in)
{
	in->Tmp = in->ElecTheta - in->OldElecTheta;

	if(in->Tmp < -0.5f)
	{
		in->Tmp += 1.0f;
	}
	else if(in->Tmp > 0.5f)
	{
		in->Tmp -= 1.0f;
	}

	in->Tmp *= in->K1;

	// Low-pass filter
	in->Tmp = (in->K2 * in->Speed) + (in->K3 * in->Tmp);
	// Saturate the output

#if defined(__TMS320C28XX_TMU__)
	in->Speed = __fsat(in->Tmp, 1.0f, -1.0f);
#elif defined(__TMS320C28XX_FPU32__)
	in->Speed = (in->Tmp > 1.0f) ? 1.0f : in->Tmp;
	in->Speed = (in->Tmp < -1.0f) ? -1.0f : in->Tmp;
#else
	in->Speed = (in->Tmp > 1.0f) ? 1.0f : in->Tmp;
	in->Speed = (in->Tmp < -1.0f) ? -1.0f : in->Tmp;
#endif

	// Update the electrical angle
	in->OldElecTheta = in->ElecTheta;

	// Change motor speed for pu to rpm value
//	in->SpeedRpm = in->BaseRpm * in->Speed;

	return(in->Speed);
}


#define SPEED_FR_MACRO(v)											\
   /* Differentiator */												\
   /* Synchronous speed computation */ 								\
   v.Tmp = v.ElecTheta - v.OldElecTheta;		                    \
   if (v.Tmp < -_IQ(0.5))			                                \
     v.Tmp = v.Tmp + _IQ(1.0);                                      \
   else if (v.Tmp > _IQ(0.5))			                            \
     v.Tmp = v.Tmp - _IQ(1.0);                                      \
   v.Tmp = _IQmpy(v.K1,v.Tmp);		                                \
   /* Low-pass filter */											\
   /* Q21 = GLOBAL_Q*Q21 + GLOBAL_Q*Q21 */							\
   	v.Tmp = _IQmpy(v.K2,_IQtoIQ21(v.Speed))+_IQmpy(v.K3,v.Tmp);		\
   /* Saturate the output */ 										\
	v.Tmp=_IQsat(v.Tmp,_IQ21(1),_IQ21(-1));							\
	v.Speed = _IQ21toIQ(v.Tmp);										\
   /* Update the electrical angle */ 									\
    v.OldElecTheta = v.ElecTheta;									\
   /* Change motor speed from pu value to rpm value (GLOBAL_Q -> Q0) */	\
   /* Q0 = Q0*GLOBAL_Q => _IQXmpy(), X = GLOBAL_Q */					\
    v.SpeedRpm = _IQmpy(v.BaseRpm,v.Speed);


#endif // __SPEED_FR_H__
