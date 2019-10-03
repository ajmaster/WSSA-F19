
#include <FreeRTOS_ARM.h> 
#define BLUE      8
#define GREEN     7   
#define RED       6

String strBlink = "red\n";

int pin = RED;

/*
  Midi Parser Callback functions
*/

SemaphoreHandle_t sem;
QueueHandle_t serialCharQueue;
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
  serialCharQueue = xQueueCreate(50, sizeof(char));
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
    bool seqInput = false;
    xSemaphoreTake(sem, portMAX_DELAY);
    if (strBlink == "seq\n"){
      strBlink = blinkList[seqCount];
      seqCount++;
      SerialUSB.println("strBlink color " + strBlink);
    } else if (strBlink == "queue"){
      charToColor();
      SerialUSB.println("strBlink color " + strBlink);
      seqInput = true;
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
    if (strBlink.indexOf("white") >=0) {
      digitalWrite(RED, HIGH);
      digitalWrite(BLUE, HIGH);
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
    if (seqCount != 0 && strBlink != "queue"){
      strBlink = "seq\n";
      if (seqCount >= sizeof(blinkList)/ sizeof(blinkList[0])){
        seqCount = 0;
      }
    }
    else if (seqInput){
      strBlink = "queue";
    }
  }
}

void serialBlink (void *arg){
  while(1){
    xSemaphoreTake(sem, portMAX_DELAY);
    if (SerialUSB.available()){
      String re = SerialUSB.readString();
      SerialUSB.println(re);
      if (re.indexOf("blue") >= 0 ||re.indexOf("red") >= 0 || re.indexOf("green") >= 0 || re.indexOf("seq") >= 0){
        seqCount = 0;
        strBlink = re;
        SerialUSB.print("Light changed to ");
        SerialUSB.println(strBlink);
      }
      else {
        strBlink = "queue";
        for (int i = 0; i < re.length() -1; i++){
          char input = re.charAt(i);
          SerialUSB.println(input);
          xQueueSendToBack(serialCharQueue, &input, portMAX_DELAY);
        }
      }
      xSemaphoreGive(sem);
    }else{
      xSemaphoreGive(sem);
    }
  }
}

void charToColor(){
  char color;
  if (uxQueueMessagesWaiting(serialCharQueue) > 0){
    xQueueReceive(serialCharQueue, &(color), portMAX_DELAY);
    SerialUSB.println(color);
    if (color == 'r' || color == 'R'){
      strBlink = "red";
    }
    else if (color == 'b' || color == 'B'){
      strBlink = "blue";
    }
    else if (color == 'g' || color == 'G'){
      strBlink = "green";
    }
    else if (color == 'w' || color == 'W'){
      strBlink = "white";
    }
    else{
      SerialUSB.print("Bad input: ");
      SerialUSB.print(color);
      SerialUSB.println("\n Defaulting to red");
      strBlink = "red";
    }
  }
  else {
    SerialUSB.println("No more values in queue\n Defaulting to red");
      strBlink = "red";
  }
}
  
