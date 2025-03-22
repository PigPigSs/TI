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

//! \file   \libraries\vib\adxl355\include\adxl355.c
//! \brief  Contains the various functions related to the adxl355 object
//!

// **************************************************************************
// the includes
#include "adxl355.h"

// **************************************************************************
// drivers



// **************************************************************************
// modules

// **************************************************************************
// platforms

// **************************************************************************
// the defines

// **************************************************************************
// the globals

// **************************************************************************
// the function prototypes


ADXL355_Handle ADXL355_init(void *pMemory)
{
    ADXL355_Handle handle;
    ADXL355_Obj *obj;

    // assign the handle
    handle = (ADXL355_Handle)pMemory;
    obj = (ADXL355_Obj *)handle;

#if defined(F28P65X_AI_EVM)
    obj->spiHandle = SPID_BASE;
#else
    obj->spiHandle = SPIB_BASE;
#endif

    obj->ADXL355_HighPass = ADXL355_NO_HPF;
    obj->ADXL355_LowPass = ADXL355_ODR_4000;
    obj->ADXL355_Range = ADXL355_RANGE_8G;

    return(handle);
} // end of ADXL355_init() function


void ADXL355_setupSPI(ADXL355_Handle handle)
{
    ADXL355_Obj *obj = (ADXL355_Obj *)handle;

    // Must put SPI into reset before configuring it
    SPI_disableModule(obj->spiHandle);

    // SPI configuration. Use a 500KHz/1MHz/2MHz SPICLK and 8-bit word size, 25/30MHz LSPCLK
    SPI_setConfig(obj->spiHandle, DEVICE_LSPCLK_FREQ, SPI_PROT_POL0PHA0,
                  SPI_MODE_CONTROLLER, 2000000, ADXL355_DATA_WIDTH);       // 2.0MHz

    SPI_disableLoopback(obj->spiHandle);

    SPI_setEmulationMode(obj->spiHandle, SPI_EMULATION_FREE_RUN);

#if defined (USE_FIFO)
    SPI_enableFIFO(obj->spiHandle);
    SPI_resetTxFIFO(obj->spiHandle);
    SPI_resetRxFIFO(obj->spiHandle);
#else
    SPI_disableFIFO(obj->spiHandle);
#endif


    // Configuration complete. Enable the module.
    SPI_enableModule(obj->spiHandle);

    return;
}  // end of ADXL355_setupSPI() function



void ADXL355_writeRegister(ADXL355_Handle handle, uint16_t wr_address, uint16_t wr_data){
    uint16_t spi_tx_data[2];
    uint16_t data_rx_line;

    spi_tx_data[0] = ((wr_address << 1) | ADXL355_WRITE) << 8;
    spi_tx_data[1] = wr_data << 8;
    ADXL355_Obj *obj = (ADXL355_Obj *)handle;

    SPI_CS_LOW;

    SPI_writeDataBlockingNonFIFO(obj->spiHandle, spi_tx_data[0]);
    data_rx_line = SPI_readDataBlockingNonFIFO(obj->spiHandle);


    SPI_writeDataBlockingNonFIFO(obj->spiHandle, spi_tx_data[1]);
    data_rx_line = SPI_readDataBlockingNonFIFO(obj->spiHandle);


    SPI_CS_HIGH;

    return;
}  // end of ADXL355_writeData() function

uint32_t ADXL355_readRegister(ADXL355_Handle handle, uint16_t rd_address, ADXL355_Read_Num_e num)
{
    uint32_t spi_rx_data;
    uint32_t rx_data_temp;


    ADXL355_Obj *obj = (ADXL355_Obj *)handle;

    SPI_CS_LOW;
    //DEVICE_DELAY_US(1);

    SPI_writeDataBlockingNonFIFO(obj->spiHandle, (((rd_address << 1) | ADXL355_READ) << 8));
    rx_data_temp = SPI_readDataBlockingNonFIFO(obj->spiHandle);

    if (num == READ_1_BYTE){
        SPI_writeDataBlockingNonFIFO(obj->spiHandle, (((rd_address << 1) | ADXL355_READ) << 8));
        rx_data_temp = SPI_readDataBlockingNonFIFO(obj->spiHandle);
        spi_rx_data = rx_data_temp;
    }
    else if(num == READ_2_BYTE){
        SPI_writeDataBlockingNonFIFO(obj->spiHandle, 0xFF00);  // write a dummy data for read
        rx_data_temp = (SPI_readDataBlockingNonFIFO(obj->spiHandle) & 0xFF);
        spi_rx_data = rx_data_temp << 8;

        SPI_writeDataBlockingNonFIFO(obj->spiHandle, 0xFF00);
        rx_data_temp = (SPI_readDataBlockingNonFIFO(obj->spiHandle) & 0xFF);
        spi_rx_data |= rx_data_temp;
    }
    else if (num == READ_3_BYTE){
        SPI_writeDataBlockingNonFIFO(obj->spiHandle, 0xFF00);
        rx_data_temp = (SPI_readDataBlockingNonFIFO(obj->spiHandle) & 0xFF);
        spi_rx_data = rx_data_temp << 16;

        SPI_writeDataBlockingNonFIFO(obj->spiHandle, 0xFF00);
        rx_data_temp = (SPI_readDataBlockingNonFIFO(obj->spiHandle) & 0xFF);
        spi_rx_data |= rx_data_temp << 8;

        SPI_writeDataBlockingNonFIFO(obj->spiHandle, 0xFF00);
        rx_data_temp = (SPI_readDataBlockingNonFIFO(obj->spiHandle) & 0xFF);
        spi_rx_data |= rx_data_temp;

    }
    else spi_rx_data = 0;

    SPI_CS_HIGH;

    return spi_rx_data;
}  // end of ADXL355_readData() function

void ADXL355_setupFilter(ADXL355_Handle handle){

    ADXL355_Obj *obj = (ADXL355_Obj *)handle;

    uint16_t filter_val = (obj->ADXL355_HighPass << 4) | obj->ADXL355_LowPass;

    ADXL355_writeRegister(obj, ADXL355_FILTER, filter_val);
}

void ADXL355_setupRange(ADXL355_Handle handle){

    ADXL355_Obj *obj = (ADXL355_Obj *)handle;
    ADXL355_writeRegister(obj, ADXL355_RANGE, obj->ADXL355_Range);
}

void ADXL355_Startup(ADXL355_Handle handle){

    ADXL355_Obj *obj = (ADXL355_Obj *)handle;
    uint16_t c_status = ADXL355_readRegister(obj, ADXL355_POWER_CTL, READ_1_BYTE) & 0xFF;

    c_status &= ~(0x01);

    ADXL355_writeRegister(obj, ADXL355_POWER_CTL, c_status);
}

void ADXL355_Standby(ADXL355_Handle handle){

    ADXL355_Obj *obj = (ADXL355_Obj *)handle;
    uint16_t c_status = ADXL355_readRegister(obj, ADXL355_POWER_CTL, READ_1_BYTE) & 0xFF;

    c_status |= 0x01;

    ADXL355_writeRegister(obj, ADXL355_POWER_CTL, c_status);
}

int32_t ADXL355_AccDataConv(uint32_t raw_data){

    int32_t data_converted;

    raw_data = raw_data & 0x000FFFFF;                // raw data is 20-bit
    if((raw_data & 0x00080000) == 0x00080000){       // negative data
       data_converted = (raw_data | 0xFFF00000);
    }
    else{
        data_converted = raw_data;
    }

    return data_converted;

}



// end of file
