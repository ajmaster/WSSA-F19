#include <Arduino.h>
#include<SPI.h>
#include<math.h>
#include <stdint.h>

// Chip Select Pin for SPI
//#define CS_PIN 51

#define CS_PIN 4


//Function Declarations
void spi_write_cmd(uint8_t address, uint8_t tx_data);
uint8_t spi_read_cmd(uint8_t address);
