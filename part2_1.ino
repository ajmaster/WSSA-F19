
#include <FreeRTOS_ARM.h> 
#define BLUE      8
#define GREEN     7   
#define RED       6


String strBlink = "red\n";

void setup() {
  SerialUSB.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(GREEN, LOW);
  while (!SerialUSB);
  xTaskCreate(serialBlink, "serialBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(timeBlink, "timeBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  vTaskStartScheduler();
  while(1);
}

void loop() {

}

SemaphoreHandle_t sem = xSemaphoreCreateBinary();
void timeBlink (void *arg){
  while(1){
    xSemaphoreTake(sem, portMAX_DELAY);
    int pin;
    if (strBlink == "red\n"){
      pin = RED;
    }
    else if (strBlink == "blue\n"){
      pin = BLUE;
    }
    else {
      pin = GREEN;
    }
    digitalWrite(pin, HIGH);
    vTaskDelay((500L * configTICK_RATE_HZ)/ 1000L);
    digitalWrite(pin, LOW);
    vTaskDelay((500L * configTICK_RATE_HZ)/ 1000L);
    xSemaphoreGive(sem);
  }
}

void serialBlink (void *arg){
  while(1){
    xSemaphoreTake(sem, portMAX_DELAY);
    if (SerialUSB.available()){
      String re = SerialUSB.readString();
      SerialUSB.println(re);
      SerialUSB.println(re == "b\n");
      if (re == "blue\n" || re == "red\n" || re == "green\n"){
        strBlink = re;
        SerialUSB.print("Light changed to ");
        SerialUSB.println(strBlink);
      }
    }
    xSemaphoreGive(sem);
  }
}
