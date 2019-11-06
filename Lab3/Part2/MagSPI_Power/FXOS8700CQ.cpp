#include "FXOS8700CQ.h"
#include <math.h>

//******************************************************************************
// Public Function Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// FXOS8700CQ(): Initialize configuration parameters
//------------------------------------------------------------------------------
FXOS8700CQ::FXOS8700CQ() {
		magODR = MODR_100HZ; // Magnetometer data/sampling rate
		magOSR = MOSR_5;     // Choose magnetometer oversample rate

    pinMode(CS_PIN, OUTPUT);        // Select the GPIO Pin 51 as SPI Chip Select
    digitalWrite(CS_PIN, HIGH);     // Set Pin to high (Active Low)
}

//------------------------------------------------------------------------------
// writeReg(): Writes to a register
//------------------------------------------------------------------------------
void FXOS8700CQ::writeReg(uint8_t reg, uint8_t data) {
  spi_write_cmd(reg, data);
}

//------------------------------------------------------------------------------
// readReg(): Reads from a register
//------------------------------------------------------------------------------
uint8_t FXOS8700CQ::readReg(uint8_t reg) {
  return (spi_read_cmd(reg));
}

//------------------------------------------------------------------------------
// readMagData(): Read the magnometer X, Y and Z axisdata
//------------------------------------------------------------------------------
void FXOS8700CQ::readMagData() {
  uint8_t msb, lsb;

  magData.x = readReg(FXOS8700CQ_M_OUT_X_LSB);
  magData.x |= readReg(FXOS8700CQ_M_OUT_X_MSB);

  SerialUSB.println(magData.x);
}
//------------------------------------------------------------------------------
// standby(): Put the FXOS8700CQ into standby mode for writing to registers
//------------------------------------------------------------------------------
void FXOS8700CQ::standby() {
  uint8_t register1 = readReg(FXOS8700CQ_CTRL_REG1);
  // place in standby mode.
  writeReg(FXOS8700CQ_CTRL_REG1, register1 & 0xFE);
}

//------------------------------------------------------------------------------
// active(): Put the FXOS8700CQ into active mode to output data
//------------------------------------------------------------------------------
void FXOS8700CQ::active() {
  uint8_t register1 = readReg(FXOS8700CQ_CTRL_REG1);
  // place in active mode.
  writeReg(FXOS8700CQ_CTRL_REG1, register1 | 0x1);
}

//------------------------------------------------------------------------------
// init(): Initialize the magnetometer
//         This function will put the magnetometer in standby mode, modify the 
//         registers that put the device in mag-only mode, set the correct data
//         rate (ODR) and oversampling rate (OSR) for the magnetometer and put
//         it back in active mode
//------------------------------------------------------------------------------
void FXOS8700CQ::init() {
  
    SPI.beginTransaction(SPISettings (1000000, MSBFIRST, SPI_MODE0));
    standby();
    spi_write_cmd(FXOS8700CQ_M_CTRL_REG1,((magOSR << 2)| 0x01));
    spi_write_cmd(FXOS8700CQ_CTRL_REG1, (magODR << 3) );
    spi_write_cmd(FXOS8700CQ_CTRL_REG2, 0x00);
    active();
    
}

//------------------------------------------------------------------------------
// checkWhoAmI(): Check the whoAmI register
//------------------------------------------------------------------------------
#define FXOS8700CQ_WHO_AM_I_VAL 0xC7
void FXOS8700CQ::checkWhoAmI(void) {
  whoAmIData = readReg(FXOS8700CQ_WHO_AM_I);
  if( whoAmIData != FXOS8700CQ_WHO_AM_I_VAL )
  {
      SerialUSB.println("WHO_AM_I check has failed");
      SerialUSB.println(whoAmIData, 4);
  }
  else
  {
      SerialUSB.println("WHO_AM_I check has passed");    
  }
}

//*****************************************************************************
