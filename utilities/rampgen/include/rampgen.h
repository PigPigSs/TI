//#############################################################################
//
//  FILE:  rampgen.h
//
//  TITLE: Ramp Generator Module
//
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

#ifndef RAMPGEN_H
#define RAMPGEN_H

#ifdef __cplusplus
extern "C" {
#endif

//*****************************************************************************
//
//! \addtogroup RAMPGEN
//! @{
//
//*****************************************************************************

//
// Included Files
//
#include <stdint.h>
#ifndef __TMS320C28XX_CLA__
#include <math.h>
#else
#include <CLAmath.h>
#endif

//#############################################################################
//
// Macro Definitions
//
//#############################################################################
#ifndef C2000_IEEE754_TYPES
#define C2000_IEEE754_TYPES
#ifdef __TI_EABI__
typedef float         float32_t;
typedef double        float64_t;
#else // TI COFF
typedef float         float32_t;
typedef long double   float64_t;
#endif // __TI_EABI__
#endif // C2000_IEEE754_TYPES

//
// Typedefs
//

//! \brief          Defines the Ramp Signal Generator (RAMPGEN) structure
//!
//! \details        The rampgen can be used to generate a ramp signal 
//!                 of a desired frequency
//!
typedef volatile struct {
    float32_t  freq;        // Input: Ramp frequency (pu)
    float32_t  stepAngleMax;// Parameter: Maximum step angle (pu)
    float32_t  out;         // Output: Ramp signal (pu)
} RAMPGEN;

//! \brief      resets RAMPGEN internal storage data and outputs
//! \param v    a pointer to a RAMPGEN structure
//!
static inline void RAMPGEN_reset(RAMPGEN *v)
{
    v->out=0;
}

//! \brief      configures RAMPGEN module
//! \param v    a pointer to a RAMPGEN structure
//! \param isrFrequency Frequency at which the RAMGEN is called in ISR
//! \param rampFrequency Frequency of the RAMP generated by RAMPGEN module
//!
static inline void RAMPGEN_config(RAMPGEN *v,
                                 float32_t isrFrequency,
                                 float32_t rampFrequency)
{
    v->freq=rampFrequency;
    v->stepAngleMax=((float32_t)1.0)/isrFrequency;
}

//! \brief      Run RAMPGEN module
//! \param v    a pointer to a RAMPGEN structure
//!
static inline void RAMPGEN_run(RAMPGEN *v)
{
    //
    // Compute the angle rate
    //
    v->out += (v->stepAngleMax*v->freq);
    
    //
    // Saturate the angle rate within (0,1)
    //
    if (v->out>(1.0f))
    {
        v->out -= (1.0f);
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif // extern "C"


#endif // end of  _RAMPGEN_H_ definition

//
// End of File
//

