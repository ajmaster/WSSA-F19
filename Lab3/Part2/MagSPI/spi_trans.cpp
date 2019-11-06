// Reading and Writing the SPI bus

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include "spi_trans.h"

//******************************************************************************
// Function Definitions
//------------------------------------------------------------------------------
#define GENERIC_SPI_TRANSFER_MODE 1
#define FXOS8700_SPI_TRANSFER_MODE 2
#define SPI_TRANSFE_MODE FXOS8700_SPI_TRANSFER_MODE
// we are not interested in the generic mode case here
// In generic mode: only control clk, mosi/miso; not the chipselect CS_PIN, let others control it
//------------------------------------------------------------------------------
// spi_write_cmd(): Transmit to a SPI device
//------------------------------------------------------------------------------
void spi_write_cmd(uint8_t address, uint8_t tx_data) {
// write to spi bus 8 bit data to address refer page 17 of spec pdf 
#if SPI_TRANSFE_MODE == FXOS8700_SPI_TRANSFER_MODE
    uint8_t address_bit7 = 0;
    address_bit7 = address & (0x1 <<7) ;
    // ( 0x01<< 7) is write bit
    address = (0x7F & address ) | ( 0x01<< 7);  
    digitalWrite(CS_PIN, LOW);     
    //Send register address
    SPI.transfer(address); 
    SPI.transfer(address_bit7);  
    //Send value to reg
    SPI.transfer(tx_data);  
    digitalWrite(CS_PIN, HIGH);     
#else
// GENERIC MODE: send what ever the address on the spi bus, assume REA/WRITE is already made in the address value
 //   digitalWrite(CS_PIN, LOW);     
    //Send register address
    SPI.transfer(address); 
    //Send value to reg
    SPI.transfer(tx_data);  
#endif
}

//------------------------------------------------------------------------------
// spi_read_cmd(): Read from a SPI device. Return the data read from register
//------------------------------------------------------------------------------
uint8_t spi_read_cmd(uint8_t address) {
//  read data from dpi bus 8 bit data from address
// pg 17 of pdf spec
    uint8_t rx_data = 0;
#if SPI_TRANSFE_MODE == FXOS8700_SPI_TRANSFER_MODE
    uint8_t address_bit7 = 0;
    address_bit7 = address & (0x1 <<7) ; 
    address = (0x7F & address );    
    digitalWrite(CS_PIN, LOW);      
    //Send register address
    SPI.transfer(address);      
    //Send 2nd byte of address 7th bit
    SPI.transfer(address_bit7);  
  // one could use  SPI.transfer(address & 0x7F);  SPI.transfer(address & 0x80); to not add more temp var
    //receive value
    rx_data = SPI.transfer(0x00);   
    digitalWrite(CS_PIN, HIGH);     
#else
// GENERIC MODE: send what ever the address on the spi bus, assume READ/WRITE is already made in the address value
//    digitalWrite(CS_PIN, LOW);     
    //Send register address
    SPI.transfer(address); 
    //receive 
    rx_data = SPI.transfer(0x00);   
//    digitalWrite(CS_PIN, HIGH);     
#endif
    return rx_data;
}

//*****************************************************************************
