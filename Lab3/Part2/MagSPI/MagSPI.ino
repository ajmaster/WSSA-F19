#include <SPI.h>
#include "FXOS8700CQ.h"

FXOS8700CQ sensor;

void setup() {
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  // Initialize SerialUSB 
  SerialUSB.begin(9600);
  while(!SerialUSB);
  SerialUSB.println("Setup SPI");
  
  // Initialize SPI
  SPI.begin();

  // Initialize sensor
  sensor = FXOS8700CQ();
  sensor.init();
  //10 ms delay
  delay(10); 
  
  // check WhoAmI
  sensor.checkWhoAmI();

}

void loop() {

      sensor.readMagData();
      SerialUSB.println("Mag X Y Z:");
      SerialUSB.println(sensor.magData.x);
      SerialUSB.println(sensor.magData.y);
      SerialUSB.println(sensor.magData.z);
      // 1000 ms delay
      delay(1000); 
    
}
