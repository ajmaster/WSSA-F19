#include <Arduino.h>
#include<SPI.h>
#include<math.h>
#include <stdint.h>

// Chip Select Pin for SPI
#define CS_PIN 4

#define SPI_MODE SPI_MODE0 
#define SPI_HZ 1000000
#define SPI_ORDER MSBFIRST


//Function Declarations
void spi_write_cmd(uint8_t address, uint8_t tx_data);
uint8_t spi_read_cmd(uint8_t address);
