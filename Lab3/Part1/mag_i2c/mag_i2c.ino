// I2C interface by default
//
#include "Wire.h"
#include "SparkFunIMU.h"
#include "SparkFunLSM303C.h"
#include "LSM303CTypes.h"
#include <FreeRTOS_ARM.h>

// #define DEBUG 1 in SparkFunLSM303C.h turns on debugging statements.
// Redefine to 0 to turn them off.

LSM303C myIMU;
float magX;
float magY;
float magZ;
SemaphoreHandle_t sem_r, sem_w;

  
static void ReadValues(void* arg){
  //read magnetometer values
  while(1){

    xSemaphoreTake(sem_r, portMAX_DELAY);
    
    magX = myIMU.readMagX();
    magY = myIMU.readMagY();
    magZ = myIMU.readMagZ();
 
    xSemaphoreGive(sem_w);
  }
}

static void PrintValues(void* arg){
  //print magnetometer values to serial port
  while(1){
    xSemaphoreTake(sem_w, portMAX_DELAY);
    
    SerialUSB.println("Magnetometer X: ");
    SerialUSB.println(magX, 4);
    SerialUSB.println("Magnetometer Y: ");
    SerialUSB.println(magY, 4);
    SerialUSB.println("Magnetometer Z: ");
    SerialUSB.println(magZ, 4);
    xSemaphoreGive(sem_r);
  }
}

uint8_t mag_id;

void setup()
{
  portBASE_TYPE s1, s2;
  while(!SerialUSB);
  SerialUSB.begin(115200);
  
  if (myIMU.begin() != IMU_SUCCESS)
    {
      SerialUSB.println("Failed setup.");
      while(1);
    }
  if( myIMU.mag_whoami() != IMU_SUCCESS)
  {
    SerialUSB.println("MAG WHO_AM_I not read success.");
  }
    SerialUSB.println("MAG WHO_AM_I id =%0x");
    SerialUSB.println(mag_id, 1);
 
  //input_q = xQueueCreate(3, 4);

// initialize semaphore
  sem_r = xSemaphoreCreateCounting(1, 1);
  sem_w = xSemaphoreCreateCounting(1, 0);

  // create task to read user input
  s1 = xTaskCreate(ReadValues, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);

  s2 = xTaskCreate(PrintValues, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  
  vTaskStartScheduler();
  //Serial.println("Insufficient RAM");
  while(1); 
}

void loop()
{
}
