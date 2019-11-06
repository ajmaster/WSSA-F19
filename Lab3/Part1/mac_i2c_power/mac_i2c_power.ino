// I2C interface by default
//
#include "Wire.h"
#include "SparkFunIMU.h"
#include "SparkFunLSM303C.h"
#include "LSM303CTypes.h"

#define LED_RGB_RED 6
#define LED_RGB_GREEN 7
#define PD_RED 1
#define PD_GREEN 2

#define pd_rgb_led(color) { \
  if(color == PD_RED){ \
    digitalWrite(LED_RGB_RED, HIGH); \
  }else{ \
    digitalWrite(LED_RGB_RED, LOW); \
  } \
  if(color == PD_GREEN){ \
    digitalWrite(LED_RGB_GREEN, HIGH); \
  }else{ \
    digitalWrite(LED_RGB_GREEN, LOW); \
  } \
}
LSM303C myIMU;
uint32_t  loopCount = 0;
int countMax=10000;
void setup()
{
  while(!SerialUSB);
  SerialUSB.begin(115200);

  if (myIMU.begin() != IMU_SUCCESS)
  {
    SerialUSB.println("Failed setup.");
    while(1);
  }
    loopCount = 0;
    countMax=10000;
    pd_rgb_led(PD_GREEN);

}

// read one register for power measurement

void loop()
{
    if( loopCount < countMax)
    {
	    myIMU.readAccelX();
    }
    else
    {
      pd_rgb_led(PD_RED);
    }
    // 10 ms delay before next loop
    delay(10); 
    loopCount++; 
}
