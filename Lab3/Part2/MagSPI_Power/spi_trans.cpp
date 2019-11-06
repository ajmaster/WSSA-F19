// Reading and Writing the SPI bus

//------------------------------------------------------------------------------
// Headers
//------------------------------------------------------------------------------
#include "spi_trans.h"

//******************************************************************************
// Function Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// spi_write_cmd(): Transmit to a SPI device
//------------------------------------------------------------------------------
void spi_write_cmd(uint8_t address, uint8_t tx_data) {

  digitalWrite(CS_PIN, LOW);

  SPI.beginTransaction(SPISettings(SPI_HZ, SPI_ORDER, SPI_MODE));

  address = (0x7F & address) | (0x01 << 7);
  SPI.transfer(address);
  SPI.transfer(tx_data);

  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();
}

//------------------------------------------------------------------------------
// spi_read_cmd(): Read from a SPI device. Return the data read from register
//------------------------------------------------------------------------------
uint8_t spi_read_cmd(uint8_t address) {
  
  uint8_t incoming_data;

  digitalWrite(CS_PIN, LOW);

  //SPI.beginTransaction(SPISettings(SPI_HZ, SPI_ORDER, SPI_MODE));

  incoming_data = SPI.transfer(0);

  SPI.transfer(0X7F & address);
  SPI.transfer(address);

  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();

  return incoming_data;
}

//*****************************************************************************
