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

#ifndef _ENCODER_H_
#define _ENCODER_H_

//! \file   \libraries\observers\encoder\include\encoder.h
//! \brief  Contains the public interface to the 
//!         encoder object
//!

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \defgroup ENCODER ENCODER
//! @{
//
//*****************************************************************************

// the includes
#ifdef __TMS320C28XX_CLA__
#include "libraries/math/src/float/CLAmath.h"
#else
#include <math.h>
#endif

#include "libraries/math/include/math.h"

//modules
#include "userParams.h"
#include "device.h"

// **************************************************************************
// the defines
//#define ENC_CALIB       1

// **************************************************************************
// the typedefs

// State machine typedef for ENC status
typedef enum  //编码器状态
{
    ENC_IDLE             = 0, //空闲
    ENC_ALIGNMENT        = 1, //对齐
    ENC_WAIT_FOR_INDEX   = 2, //等待
    ENC_CALIBRATION_DONE = 3 //校准完成
} ENC_Status_e;


//! \brief Defines the ENC controller object
//!
typedef struct _ENC_Obj_  //编码器对象结构体
{
    float32_t Ts_sec;               // sampling period (sec) //采样周期
	float32_t polePairs;            // pole pairs of the motor //电机极对数
	float32_t encLines;             // lines of the encoder //编码器线数

    float32_t mechanicalScaler;

    float32_t thetaMech_pu;         // Mechanical angle //机械角度(每个单元)
    float32_t thetaMech_rad;        // Mechanical angle //机械角度(弧度)
    float32_t thetaElec_rad;        // Electrical angle //电角度
    float32_t thetaHall_rad;        // Initial angle of the hall sensor //初始化霍尔传感器角度
    float32_t *ptrHalltheta;
    uint32_t  *ptrHallPos;

    float32_t speedElec_Hz;         // target speed //目标速度
    float32_t speedMech_Hz;         // estimated rotor speed //测量转速

    uint32_t  qepHandle;            // the QEP handle //正交脉冲句柄
    uint32_t  indexOffset;          // the offset of index 

#if defined(ENC_CALIB)
    float32_t hallThetaData[12];
    uint32_t  hallPosData[12];
    uint16_t  hallStateIndex[12];
    uint16_t  hallIndex;
    uint16_t  hallStatePrev;
#endif  // ENC_CALIB

    uint16_t  gpioHallU;            // the GPIO for Hall U
    uint16_t  gpioHallV;            // the GPIO for Hall V
    uint16_t  gpioHallW;            // the GPIO for Hall W
    uint16_t  hallState;
    uint16_t  hallStateZero;

    ENC_Status_e encState;
} ENC_Obj;


//! \brief Defines the ENC handle
//!
typedef struct _ENC_Obj_ *ENC_Handle;

// **************************************************************************
// the function prototypes
//! \brief     Initializes the ENC controller //初始化编码器控制器
//! \param[in] pMemory   A pointer to the memory for the ENC controller object //编码器对象指针
//! \param[in] numBytes  The number of bytes allocated for the ENC controller object, bytes //编码器控制控制器对象分配字节数
//! \return The ENC controller (ENC) object handle //返回编码器控制器对象句柄
extern ENC_Handle ENC_init(void *pMemory, const size_t numBytes);

//! \brief     Set the controller
//! \param[in] handle      The ENC controller handle
extern void ENC_setParams(ENC_Handle handle, const USER_Params *pUserParams);

//! \brief     Runs the ENC controller
//! \param[in] handle      The ENC controller handle
//! \return    speed from encoder //返回速度的句柄
static inline float32_t ENC_getSpeedElec_Hz(ENC_Handle handle)
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    return(obj->speedElec_Hz);
}

//! \brief     gets the angle from encoder
//! \param[in] handle      The ENC controller handle
//! \return    angle from encoder //返回电角度
static inline float32_t ENC_getElecAngle(ENC_Handle handle)
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    return(obj->thetaElec_rad);
}

//! \brief     Gets the state of the ENC controller
//! \param[in] handle   the ENC Handle
//! \return    state from encode //返回编码器状态
static inline float32_t ENC_getState(ENC_Handle handle)
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    return(obj->encState);
}

//! \brief     Gets     the ENC controller
//! \param[in] handle   the ENC Handle
static inline void ENC_resetState(ENC_Handle handle) //根据参数名称为编码器状态重置
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    obj->encState = ENC_IDLE;

    return;
}

//! \brief     sets up the ENC controller
//! \param[in] handle   the ENC Handle
static inline void ENC_setState(ENC_Handle handle, const ENC_Status_e encState) //根据参数名称为编码器状态设定
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    obj->encState = encState;

    return;
}

//! \brief     set     the ENC controller
//! \param[in] handle  the ENC Handle //设定霍尔传感器通用接口
extern void ENC_setHallGPIO(ENC_Handle handle, const uint16_t gpioHallU,
                     const uint16_t gpioHallV, const uint16_t gpioHallW);

//! \brief     set     the ENC controller
//! \param[in] handle  the ENC Handle
static inline void ENC_setGPIOHallU(ENC_Handle handle, const uint32_t gpioHallU) //U相霍尔传感器
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    obj->gpioHallU = gpioHallU;

    return;
}

//! \brief     Gets     the ENC controller
//! \param[in] handle   the ENC Handle
static inline void ENC_setGPIOHallV(ENC_Handle handle, const uint32_t gpioHallV) //V相霍尔传感器
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    obj->gpioHallV = gpioHallV;

    return;
}

//! \brief     Gets     the ENC controller
//! \param[in] handle   the ENC Handle
static inline void ENC_setGPIOHallW(ENC_Handle handle, const uint32_t gpioHallW) //W相霍尔传感器
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    obj->gpioHallW = gpioHallW;

    return;
}

//! \brief     Gets     the ENC controller
//! \param[in] handle   the ENC Handle
static inline void ENC_setQEPHandle(ENC_Handle handle, const uint32_t qepBase) //设置正交脉冲的方式
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    obj->qepHandle = qepBase;

    return;
}

//! \brief     get     the hall state
//! \param[in] handle  the ENC Handle
static inline uint16_t ENC_getHallState(ENC_Handle handle) //获取霍尔传感器状态
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    uint16_t hallState = 0;

    hallState = GPIO_readPin(obj->gpioHallU);
    hallState += GPIO_readPin(obj->gpioHallV)<<1;
    hallState += GPIO_readPin(obj->gpioHallW)<<2;

    return(hallState & 0x0007);
}

//! \brief     Runs the ENC controller
//! \param[in] handle  the ENC Handle
extern void ENC_full_run(ENC_Handle handle); //运行编码器控制器

//! \brief     Runs the ENC controller
//! \param[in] handle  the ENC Handle
#define ENC_inline_run ENC_run

static inline void ENC_run(ENC_Handle handle)
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    if(obj->encState == ENC_CALIBRATION_DONE)
    {
        obj->thetaMech_pu = obj->mechanicalScaler *
                (float32_t)EQEP_getPositionLatch(obj->qepHandle); //将编码器位置信息以一定的比例转换

        obj->thetaMech_rad = obj->thetaMech_pu * MATH_TWO_PI; //将编码器单位值转换为弧度制

        float32_t thetaElec_pu = obj->thetaMech_pu * obj->polePairs;

        obj->thetaElec_rad = (thetaElec_pu - ((int32_t)thetaElec_pu)) * MATH_TWO_PI;

        if(obj->thetaElec_rad >= MATH_PI)
        {
            obj->thetaElec_rad = obj->thetaElec_rad - MATH_TWO_PI;
        }
        else if(obj->thetaElec_rad <= -MATH_PI)
        {
            obj->thetaElec_rad = obj->thetaElec_rad + MATH_TWO_PI;
        }
    }
    else if(obj->encState == ENC_WAIT_FOR_INDEX)
    {
        if(EQEP_getInterruptStatus(obj->qepHandle) & EQEP_INT_INDEX_EVNT_LATCH)
        {
            EQEP_setInitialPosition(obj->qepHandle, EQEP_getPositionLatch(obj->qepHandle));

            EQEP_setPositionInitMode(obj->qepHandle, EQEP_INIT_RISING_INDEX);

            obj->indexOffset = EQEP_getPositionLatch(obj->qepHandle);
            obj->encState = ENC_CALIBRATION_DONE;
        }
    }
    else    // obj->encState == ENC_ALIGNMENT
    {

#if defined(ENC_CALIB)
        obj->hallStateZero = ENC_getHallState(handle);
        obj->hallIndex = 0;
#endif  // ENC_CALIB

        // during alignment, reset the current shaft position to zero
        EQEP_setPosition(obj->qepHandle, 0);

        // Reset pos cnt for QEP
        EQEP_clearInterruptStatus(obj->qepHandle, EQEP_INT_INDEX_EVNT_LATCH);

        // reset poscnt init on index
        EQEP_setPositionInitMode(obj->qepHandle, EQEP_INIT_DO_NOTHING);
    }

#if defined(ENC_CALIB)
    obj->hallState = ENC_getHallState(handle);

    if(obj->hallState != obj->hallStatePrev)
    {
        obj->hallThetaData[obj->hallIndex] = obj->thetaElec_rad;
        obj->hallPosData[obj->hallIndex] = EQEP_getPositionLatch(obj->qepHandle);
        obj->hallStateIndex[obj->hallIndex] = obj->hallState;

        obj->hallIndex++;

        if(obj->hallIndex >= 12)
        {
            obj->hallIndex = 0;
        }
    }

    obj->hallStatePrev = obj->hallState;
#endif  // ENC_CALIB

    return;
}


//! \brief     Runs the hall of ENC controller
//! \param[in] handle  The ENC controller handle
static inline void ENC_runHall(ENC_Handle handle)
{
    ENC_Obj *obj = (ENC_Obj *)handle;

    if(obj->encState == ENC_CALIBRATION_DONE)
    {
        obj->thetaMech_pu = obj->mechanicalScaler *
                (float32_t)EQEP_getPositionLatch(obj->qepHandle);

        obj->thetaMech_rad = obj->thetaMech_pu * MATH_TWO_PI;

        float32_t thetaElec_pu = obj->thetaMech_pu * obj->polePairs;

        obj->thetaElec_rad = (thetaElec_pu - ((int32_t)thetaElec_pu)) * MATH_TWO_PI;

        if(obj->thetaElec_rad >= MATH_PI)
        {
            obj->thetaElec_rad = obj->thetaElec_rad - MATH_TWO_PI;
        }
        else if(obj->thetaElec_rad <= -MATH_PI)
        {
            obj->thetaElec_rad = obj->thetaElec_rad + MATH_TWO_PI;
        }
    }
    else if(obj->encState == ENC_WAIT_FOR_INDEX)
    {
        if(EQEP_getInterruptStatus(obj->qepHandle) & EQEP_INT_INDEX_EVNT_LATCH)
        {
            EQEP_setInitialPosition(obj->qepHandle, EQEP_getPositionLatch(obj->qepHandle));

            EQEP_setPositionInitMode(obj->qepHandle, EQEP_INIT_RISING_INDEX);

            obj->encState = ENC_CALIBRATION_DONE;
        }
    }
    else    // obj->encState == ENC_ALIGNMENT
    {
        // during alignment, reset the current shaft position to zero
        EQEP_setPosition(obj->qepHandle, 0);

        // Reset pos cnt for QEP
        EQEP_clearInterruptStatus(obj->qepHandle, EQEP_INT_INDEX_EVNT_LATCH);

        // reset poscnt init on index
        EQEP_setPositionInitMode(obj->qepHandle, EQEP_INIT_DO_NOTHING);
    }

    return;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif //end of _ENC_H_ definition

