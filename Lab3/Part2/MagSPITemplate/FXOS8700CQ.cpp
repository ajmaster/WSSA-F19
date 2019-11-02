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
//reg is the address of the register
// we can use the spi_transfer call
//void  spi_write_cmd(uint8_t address, uint8_t tx_data);
    spi_write_cmd( reg, data);
#if 0
 //or use the re-implementation  in the following
// write to spi bus 8 bit data to register reg 
// pg 17 of pdf spec
    uint8_t address_bit7 = 0;
    uint8_t address = reg; 
    address_bit7 = address & (0x1 <<7) ;
    // ( 0x01<< 7) is write bit
    address = (0x7F & address ) | ( 0x01<< 7);  
    digitalWrite(CS_PIN, LOW);
    //Send register address     
    SPI.transfer(address); 
    //Send 2nd byte of address 7th bit
    SPI.transfer(address_bit7);
    SPI.transfer(data);  
    digitalWrite(CS_PIN, HIGH);     
#endif
}

//------------------------------------------------------------------------------
// readReg(): Reads from a register
//------------------------------------------------------------------------------
uint8_t FXOS8700CQ::readReg(uint8_t reg) {
//reg is the address of the register
  return ( spi_read_cmd(reg) );
  //or use the re-implementation  in the following
#if 0
    uint8_t rx_data=0;
    uint8_t address_bit7 = 0;
    uint8_t address = reg; 
    address_bit7 = address & (0x1 <<7) ;
    address = (0x7F & address ) ;  
    digitalWrite(CS_PIN, LOW);      // Set Pin to low (Active Low)
    SPI.transfer(address);      //Send register location
    SPI.transfer(address_bit7);   //Send 2nd byte of address 7th bit
    rx_data = SPI.transfer(0x00);  //Send value 00 for read
    digitalWrite(CS_PIN, HIGH);     // Set Pin to high (Active Low)
    reurn rx_data;
#endif
}

//------------------------------------------------------------------------------
// readMagData(): Read the magnometer X, Y and Z axisdata
//------------------------------------------------------------------------------
void FXOS8700CQ::readMagData() {
  uint8_t MSB_data, LSB_data;
  uint8_t status;
    status = readReg( FXOS8700CQ_M_DR_STATUS );
    if( (status & 0x08)!= 0) // new data
    {
      MSB_data = readReg(FXOS8700CQ_M_OUT_X_MSB);
      LSB_data = readReg(FXOS8700CQ_M_OUT_X_LSB);
      magData.x = (MSB_data << 8) |  LSB_data ;
      MSB_data = readReg(FXOS8700CQ_M_OUT_Y_MSB);
      MSB_data = readReg(FXOS8700CQ_M_OUT_Y_MSB);
      magData.y = (MSB_data << 8) |  LSB_data ;
      MSB_data = readReg(FXOS8700CQ_M_OUT_Z_MSB);
      MSB_data = readReg(FXOS8700CQ_M_OUT_Z_MSB);
      magData.z = (MSB_data << 8) |  LSB_data ;
    }
}
//------------------------------------------------------------------------------
// standby(): Put the FXOS8700CQ into standby mode for writing to registers
//------------------------------------------------------------------------------
void FXOS8700CQ::standby() {
  uint8_t mode = 0;
  ctrl_reg1_save = readReg(FXOS8700CQ_CTRL_REG1);
  writeReg( FXOS8700CQ_CTRL_REG1, (ctrl_reg1_save & 0xFE  ) ); // standby 
  while( (mode = (spi_read_cmd(FXOS8700CQ_SYSMOD)& 0x3)) != 0 )
  {
    ; // wait to change to standby
  }

}

//------------------------------------------------------------------------------
// active(): Put the FXOS8700CQ into active mode to output data
//------------------------------------------------------------------------------
void FXOS8700CQ::active() {
  ctrl_reg1_save = readReg(FXOS8700CQ_CTRL_REG1);
  spi_write_cmd( FXOS8700CQ_CTRL_REG1, (ctrl_reg1_save != 0x01 ) );   // active
}

//------------------------------------------------------------------------------
// init(): Initialize the magnetometer
//         This function will put the magnetometer in standby mode, modify the 
//         registers that put the device in mag-only mode, set the correct data
//         rate (ODR) and oversampling rate (OSR) for the magnetometer and put
//         it back in active mode
//------------------------------------------------------------------------------
void FXOS8700CQ::init() {
// old style
//SPI.setDataMode (SPI_MODE0);
//SPI.setBitOrder (MSBFIRST);   // most significant bit first
// newer 1 MHZ spi clock    
  SPI.beginTransaction (SPISettings (1000000, MSBFIRST, SPI_MODE0));  // 1 MHz clock, MSB first, mode 0
  standby();
// or  spi_write_cmd( FXOS8700CQ_CTRL_REG1, 0x0);         // standby 
  spi_write_cmd( FXOS8700CQ_M_CTRL_REG1,((magOSR << 2)| 0x01)); // mag only mode, osr = 5 
  spi_write_cmd( FXOS8700CQ_CTRL_REG1, (magODR << 3) );     // assume default 0 to start, with changes here

  spi_write_cmd( FXOS8700CQ_CTRL_REG2, 0x00 ); // not enable sleep   1 << 2 ; wake up always
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
      SerialUSB.println("ID error! The whoami read is not FXOS8700CQ_WHOAMI_VAL:");
      SerialUSB.println(whoAmIData, 4);
  }
  else
  {
      SerialUSB.println("ID correct! The whoami read is FXOS8700CQ_WHOAMI_VAL.");    
  }
}

//*****************************************************************************
