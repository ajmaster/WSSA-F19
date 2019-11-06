  #include <SPI.h>
#include "FXOS8700CQ.h"
#include "PowerDueLED.h"

FXOS8700CQ sensor;
int maxLoops = 10000;
uint32_t loopCtr = 0;

void setup() {
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(8, LOW);

  // Initialize SerialUSB 
  while(!SerialUSB);
  SerialUSB.begin(9600);
  
  // Initialize SPI
  SPI.begin();

  // Initialize sensor
  sensor = FXOS8700CQ();
  sensor.init();
  delay(10);

  sensor.checkWhoAmI();
}

void loop() {
  SerialUSB.println("Loop starting");
  digitalWrite(6, HIGH);
  
  if (loopCtr < maxLoops) {
    sensor.readReg(FXOS8700CQ_WHO_AM_I);
  }
  digitalWrite(6, LOW);
  loopCtr++;
  SerialUSB.println("Loop finished");
  delay(1000);
}
