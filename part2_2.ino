
#include <FreeRTOS_ARM.h> 
#define BLUE      8
#define GREEN     7   
#define RED       6


String strBlink = "red\n";

int pin = RED;

SemaphoreHandle_t sem;

String blinkList[] = {"red", "green", "blue", "redgreen", "redblue", "greenblue", "redgreenblue"};
int seqCount = 0;
void setup() {
  SerialUSB.begin(9600);
  pinMode(RED, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(GREEN, LOW);
  while (!SerialUSB);
  sem = xSemaphoreCreateCounting(1, 0);
  xSemaphoreGive(sem);
  
  xTaskCreate(serialBlink, "serialBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  xTaskCreate(timeBlink, "timeBlink", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
  
  vTaskStartScheduler();
  while(1);
}

void loop() {

}

void timeBlink (void *arg){
  while(1){
    SerialUSB.println("timeblink");
    xSemaphoreTake(sem, portMAX_DELAY);
    if (strBlink == "seq\n"){
      strBlink = blinkList[seqCount];
      seqCount++;
    }
    if (strBlink.indexOf("red") >= 0){
      digitalWrite(RED, HIGH);
    }
    if (strBlink.indexOf("blue") >= 0){
      digitalWrite(BLUE, HIGH);
    }
    if (strBlink.indexOf("green") >= 0) {
      digitalWrite(GREEN, HIGH);
    }
    xSemaphoreGive(sem);
    vTaskDelay((500L * configTICK_RATE_HZ)/ 1000L);
    xSemaphoreTake(sem, portMAX_DELAY);
    digitalWrite(RED, LOW);
    digitalWrite(BLUE, LOW);
    digitalWrite(GREEN, LOW);
    xSemaphoreGive(sem);
    vTaskDelay((500L * configTICK_RATE_HZ)/ 1000L);
    if (seqCount != 0){
      strBlink = "seq\n";
      if (seqCount >= sizeof(blinkList)/ sizeof(blinkList[0])){
        seqCount = 0;
      }
    }
  }
}

void serialBlink (void *arg){
  while(1){
    xSemaphoreTake(sem, portMAX_DELAY);
    if (SerialUSB.available()){
      String re = SerialUSB.readString();
      if (re == "blue\n" || re == "red\n" || re == "green\n" || re == "seq\n"){
        seqCount = 0;
        strBlink = re;
        SerialUSB.print("Light changed to ");
        SerialUSB.println(strBlink);
      }
    }
    xSemaphoreGive(sem);
  }
}
