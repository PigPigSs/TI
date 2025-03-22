//#############################################################################
// $Copyright:
// Copyright (C) 2017-2023 Texas Instruments Incorporated - http://www.ti.com/
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

//! \file   \libraries\vib\adxl355\include\adxl355.h
//! \brief  Contains public interface to various functions related
//!         to the adxl355 object
//!

#ifndef ADXL355_H
#define ADXL355_H

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
//! \defgroup ADXL355 ADXL355
//! @{
//
//*****************************************************************************

// the includes
#include <math.h>

//#include "libraries/math/include/math.h"

// drivers
#include "device.h"

//#include "libraries/math/include/math.h"


// the defines
// **************************************************************************
//
//  ADXL355 registers addresses
//
// **************************************************************************
#define ADXL355_DEVID_AD                 0x00
#define ADXL355_DEVID_MST                0x01
#define ADXL355_PARTID                   0x02
#define ADXL355_REVID                    0x03
#define ADXL355_STATUS                   0x04
#define ADXL355_FIFO_ENTRIES             0x05
#define ADXL355_TEMP2                    0x06
#define ADXL355_TEMP1                    0x07
#define ADXL355_XDATA3                   0x08
#define ADXL355_XDATA2                   0x09
#define ADXL355_XDATA1                   0x0A
#define ADXL355_YDATA3                   0x0B
#define ADXL355_YDATA2                   0x0C
#define ADXL355_YDATA1                   0x0D
#define ADXL355_ZDATA3                   0x0E
#define ADXL355_ZDATA2                   0x0F
#define ADXL355_ZDATA1                   0x10
#define ADXL355_FIFO_DATA                0x11
#define ADXL355_OFFSET_X_H               0x1E
#define ADXL355_OFFSET_X_L               0x1F
#define ADXL355_OFFSET_Y_H               0x20
#define ADXL355_OFFSET_Y_L               0x21
#define ADXL355_OFFSET_Z_H               0x22
#define ADXL355_OFFSET_Z_L               0x23
#define ADXL355_ACT_EN                   0x24
#define ADXL355_ACT_THRESH_H             0x25
#define ADXL355_ACT_THRESH_L             0x26
#define ADXL355_ACT_COUNT                0x27
#define ADXL355_FILTER                   0x28
#define ADXL355_FIFO_SAMPLES             0x29
#define ADXL355_INT_MAP                  0x2A
#define ADXL355_SYNC                     0x2B
#define ADXL355_RANGE                    0x2C
#define ADXL355_POWER_CTL                0x2D
#define ADXL355_SELF_TEST                0x2E
#define ADXL355_RESET                    0x2F

// **************************************************************************
//
// Accelerometer data width
//
// **************************************************************************
#define ADXL355_DATA_WIDTH               8U

// **************************************************************************
//
// Accelerometer write/read command - to be append to the end of address
//
// **************************************************************************
#define ADXL355_WRITE                    0x0
#define ADXL355_READ                     0x1

// **************************************************************************
//
// Accelerometer scales to corresponding range
//
// **************************************************************************
#define ADXL355_RANGE_2G_SCALE           256000.0f
#define ADXL355_RANGE_4G_SCALE           128000.0f
#define ADXL355_RANGE_8G_SCALE           64000.0f

// **************************************************************************
//
// Definitions that later connect with hal.h
// SPI GPIO assignment (SPI GPIO configuration is already in hal.c)
// External interrupt definition
//
// **************************************************************************
#if defined(F28P65X_AI_EVM)
#define SPI_CLK_PIN                     93
#define SPI_PICO_PIN                    91
#define SPI_POCI_PIN                    92
#define SPI_CS_PIN                      94
#define DATA_RDY_GPIO                   25
#else
#define SPI_CLK_PIN                     58
#define SPI_SIMO_PIN                    60
#define SPI_SOMI_PIN                    61
#define SPI_CS_PIN                      27
#define SPI_CLK_PIN_CONFIG              GPIO_58_SPIB_CLK
#define SPI_SIMO_PIN_CONFIG             GPIO_60_SPIB_SIMO
#define SPI_SOMI_PIN_CONFIG             GPIO_61_SPIB_SOMI
#define SPI_STE_PIN_CONFIG              GPIO_27_SPIB_STE

#define VIB_INT_PIE_NUM                 INT_XINT3        // INT12.1 - XINT3
#define GPIO_XINT_MAP                   GPIO_INT_XINT3
#define DATA_RDY_GPIO                   26
#endif

#define SPI_CS_LOW                      GPIO_writePin(SPI_CS_PIN, 0)
#define SPI_CS_HIGH                     GPIO_writePin(SPI_CS_PIN, 1)



// the typedefs
//------------------------------------------------------------------------------
//! \brief Enumeration for the ODR values
//! LPF freq is set to ODR/4 automatically
//!
typedef enum{
    ADXL355_ODR_4000   = 0U,
    ADXL355_ODR_2000   = 1U,
    ADXL355_ODR_1000   = 2U,
    ADXL355_ODR_500    = 3U,
    ADXL355_ODR_250    = 4U,
    ADXL355_ODR_125    = 5U,
    ADXL355_ODR_62_5   = 6U,
    ADXL355_ODR_31_25  = 7U,
    ADXL355_ODR_15_625 = 8U,
    ADXL355_ODR_7_813  = 9U,
    ADXL355_ODR_3_906  = 10U
} ADXL355_ODR_e;

//! \brief Enumeration for the HPF
//! Collection of selective HPF corner frequency
//! For details refer to ADXL355 datasheet
//!
typedef enum{
    ADXL355_NO_HPF   = 0U,
    ADXL355_TYPE_I   = 1U,             // 24.7e-4 ODR
    ADXL355_TYPE_II  = 2U,             // 6.2084e-4 ODR
    ADXL355_TYPE_III = 3U,
    ADXL355_TYPE_IV  = 4U,
    ADXL355_TYPE_V   = 5U,
    ADXL355_TYPE_VI  = 6U
} ADXL355_HPF_e;


//! \brief Enumeration for the range values
//!
typedef enum{
    ADXL355_RANGE_2G = 0x81U,
    ADXL355_RANGE_4G = 0x82U,
    ADXL355_RANGE_8G = 0x83U
} ADXL355_Range_e;

//! \brief Number of bytes read
//!
typedef enum{
    READ_1_BYTE = 1,
    READ_2_BYTE = 2,
    READ_3_BYTE = 3
} ADXL355_Read_Num_e;

//! \brief Object for the ADXL355 registers and commands
//!

typedef struct _ADXL355_Obj_
{
    uint16_t ADXL355_Range;
    uint16_t ADXL355_LowPass;
    uint16_t ADXL355_HighPass;
    uint32_t spiHandle;             //!< specifies which SPI module the accelerometer is connected to
} ADXL355_Obj;


//! \brief Defines the ADXL355 handle
//!
typedef struct _ADXL355_Obj_ *ADXL355_Handle;

// **************************************************************************
// the globals

// **************************************************************************
// the function prototypes

//! \brief     Initializes the ADXL355 object
//! \param[in] pMemory   A pointer to the memory for the ADXL355 object
//! \return    The ADXL355 object handle
extern ADXL355_Handle ADXL355_init(void *pMemory);

//! \brief     Write to the Filter register in ADXL355 to complete the setup
//! \param[in] pMemory   A pointer to the memory for the ADXL355 object

extern void ADXL355_setupFilter(ADXL355_Handle handle);

//! \brief     Write to the Range register in ADXL355 to complete the setup
//! \param[in] pMemory   A pointer to the memory for the ADXL355 object

extern void ADXL355_setupRange(ADXL355_Handle handle);

//! \brief     Turns ADXL355 to measurement mode
//!
extern void ADXL355_Startup(ADXL355_Handle handle);

//!  \brief    Turns ADXL355 to standby mode (default mode after reset)
//!
extern void ADXL355_Standby(ADXL355_Handle handle);

//! \brief     setup the SPI for the ADXL355
//! \param[in] handle     The ADXL355 handle
extern void ADXL355_setupSPI(ADXL355_Handle handle);


//! \brief     Write data to the ADXL355 register
//! \param[in] handle       The DAC128S handle
//! \param[in] wr_address   The register address to write (7-bit + write identifier)
//! \param[in] wr_data      The data to write (8-bit, MSB)
extern void ADXL355_writeRegister(ADXL355_Handle handle, uint16_t wr_address, uint16_t wr_data);


//! brief      Acquire data from ADXL355 register
//! \param[in] handle       The ADXL355 handle
//! \param[in] rd_address   The address for the register to be read, if multiple bytes, then this is the start address (7-bit + read identifier)
//! \param[in] num          Number of bytes to read
//!                         1 byte: common registers
//!                         2-byte: temperature, offsets
//!                         3-byte: only available for X, Y, Z vibration data
//! \return    The read data (reserve 32-bit to the data since the biggest size is 3-byte)
extern uint32_t ADXL355_readRegister(ADXL355_Handle handle, uint16_t rd_address, ADXL355_Read_Num_e num);


//! brief      Converts the raw acceleration data into signed integers
//! \param[in] raw data from register read
//! \return    converted acceleration data
extern int32_t ADXL355_AccDataConv(uint32_t raw_data);

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
#endif // extern "C"

#endif // end of ADXL355_H definition
