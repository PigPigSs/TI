//##############################################################################
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
//##############################################################################

//------------------------------------------------------------------------------
//!
//! \file   libraries/utilities/cpu_time/source/cpu_usage.c
//! \brief  Portable C fixed point code.  These functions define the 
//!         CPU usage time (CPU_TIME) module routines
//!
//------------------------------------------------------------------------------


// **************************************************************************
// the includes

#include "cpu_time.h"


// **************************************************************************
// the globals


// **************************************************************************
// the functions

// Initializes the CPU TIME (CPU_TIME) object
CPU_TIME_Handle CPU_TIME_init(void *pMemory,const size_t numBytes)
{
    CPU_TIME_Handle handle;

    if(numBytes < sizeof(CPU_TIME_Obj))
    {
        return((CPU_TIME_Handle)NULL);
    }

    // assign the handle
    handle = (CPU_TIME_Handle)pMemory;

    return(handle);
} // end of CPU_TIME_init() function

// Resets timer count
void CPU_TIME_reset(CPU_TIME_Handle handle)
{
    CPU_TIME_Obj *obj = (CPU_TIME_Obj *)handle;
    uint16_t cnt;

    for(cnt = 0; cnt < MOD_INDEX_MAX; cnt++)
    {
        obj->deltaNow[cnt] = 0;
        obj->deltaMin[cnt] = 0x7FFF;
        obj->deltaMax[cnt] = 0;
    }

    obj->pwmCntADC = 0;
    obj->pwmCntISR = 0;

    obj->pwmCntADCMin = 0x3FFF;
    obj->pwmCntISRMin = 0x3FFF;

    obj->pwmCntADCMax = 0;
    obj->pwmCntISRMax = 0;

    obj->flag_resetStatus = true;

    return;
} // end of CPU_TIME_setParams() function

// end of file
