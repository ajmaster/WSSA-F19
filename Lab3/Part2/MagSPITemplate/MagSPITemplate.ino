#include <SPI.h>
#include "FXOS8700CQ.h"
#define LED_RGB_RED 6
#define LED_RGB_GREEN 7
#define LED_RGB_BLUE 8
#define PD_RED 1
#define PD_GREEN 2
#define PD_BLUE 3
#define PD_YELLOW 4
#define PD_PURPLE 5
#define PD_TIEL 6
#define PD_WHITE 7
#define PD_OFF 8

#define pd_rgb_led(color) { \
  if(color == PD_RED || color == PD_YELLOW || color == PD_PURPLE || color == PD_WHITE){ \
    digitalWrite(LED_RGB_RED, HIGH); \
  }else{ \
    digitalWrite(LED_RGB_RED, LOW); \
  } \
  if(color == PD_GREEN || color == PD_YELLOW || color == PD_TIEL || color == PD_WHITE){ \
    digitalWrite(LED_RGB_GREEN, HIGH); \
  }else{ \
    digitalWrite(LED_RGB_GREEN, LOW); \
  } \
  if(color == PD_BLUE || color == PD_PURPLE || color == PD_TIEL || color == PD_WHITE){ \
    digitalWrite(LED_RGB_BLUE, HIGH); \
  }else{ \
    digitalWrite(LED_RGB_BLUE, LOW); \
  } \
}

FXOS8700CQ sensor;
uint32_t  loopCount = 0;

void setup() {
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  // Initialize SerialUSB 
  while(!SerialUSB);
  SerialUSB.begin(115200);

  // Initialize SPI
  SPI.begin();

  // Initialize sensor
  sensor = FXOS8700CQ();
  sensor.init();
  // added
  sensor.checkWhoAmI();
  // to start loop
  loopCount = 0;
    pd_rgb_led(PD_GREEN);
//    sensor.active();
}

void loop() {

      sensor.readMagData();
      SerialUSB.println("Mag X Y Z:");
      SerialUSB.println(sensor.magData.x, 2);
      SerialUSB.println(sensor.magData.y, 2);
      SerialUSB.println(sensor.magData.z, 2);

      delay(1000); // 1000 ms
    
}
