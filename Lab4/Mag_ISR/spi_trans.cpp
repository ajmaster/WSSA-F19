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
// write to spi bus 8 bit data to address
// pg 17 of pdf spec
#if SPI_TRANSFE_MODE == FXOS8700_SPI_TRANSFER_MODE
	// one could use  SPI.transfer(address & 0x7F);  SPI.transfer(address & 0x80); to not add more temp var
	// here to use temp var address_bit7 to make it more explicit
    uint8_t address_bit7 = 0;
    //since there is a need for chip select then delay then clock s etc.
    //  so we call chip select and delay it
    digitalWrite(CS_PIN, LOW);     // Set Pin to low (Active Low)
    digitalWrite(CS_PIN, LOW);     // Set Pin to low (Active Low)
 //   delayMicroseconds(2); 
    address_bit7 = address & (0x1 <<7) ;
    address = (0x7F & address ) | ( 0x01<< 7);  // ( 0x01<< 7) is write bit

    SPI.transfer(address); //Send register location
    SPI.transfer(address_bit7);  //Send 2nd byte of address 7th bit
    SPI.transfer(tx_data);  //Send value to record into register
    // 65 ns delay  throu fake instruction
    address_bit7 = address & (0x80) ;
    address      = address_bit7 | (0x7F & address );
    digitalWrite(CS_PIN, HIGH);     // Set Pin to high (Active Low)
#else
// GENERIC MODE: send what ever the address on the spi bus, assume REA/WRITE is already made in the address value
 //   digitalWrite(CS_PIN, LOW);     // Set Pin to low (Active Low)
    SPI.transfer(address); //Send register location
    SPI.transfer(tx_data);  //Send value to record into register
 //   digitalWrite(CS_PIN, HIGH);     // Set Pin to high (Active Low)
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
    digitalWrite(CS_PIN, LOW);       // Set Pin to low (Active Low)
    // put CS before the following to achieve delay for clock etc., effective delay of 65 ns required
    address_bit7 = address & (0x1 <<7) ; //  & 0x80
    address = (0x7F & address );  	// 7th bit is read, 0, now
    SPI.transfer(address); 			//Send register location
    SPI.transfer(address_bit7);  //Send 2nd byte of address 7th bit
	// one could use  SPI.transfer(address & 0x7F);  SPI.transfer(address & 0x80); to not add more temp var
    rx_data = SPI.transfer(0x00);  //receive value 
    // effective delay
    address_bit7 = address & (0x1 <<7) ; //  & 0x80
    address = (0x7F & address );    // 7th bit is read, 0, now
    digitalWrite(CS_PIN, HIGH);     // Set Pin to high (Active Low)
#else
// GENERIC MODE: send what ever the address on the spi bus, assume READ/WRITE is already made in the address value
//    digitalWrite(CS_PIN, LOW);     // Set Pin to low (Active Low)
    SPI.transfer(address); //Send register location
    rx_data = SPI.transfer(0x00);  //receive value 
//    digitalWrite(CS_PIN, HIGH);     // Set Pin to high (Active Low)
#endif
    return rx_data;
}

//*****************************************************************************
