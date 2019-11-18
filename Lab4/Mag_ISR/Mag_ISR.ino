#include <SPI.h>
#include <math.h>
#include "FXOS8700CQ.h"
//#include <task.h> 
#include <FreeRTOS_ARM.h>

#define USE_SINGLE_X_MAG 1
//#define USE_SINGLE_X_MAG 0



FXOS8700CQ sensor;
uint32_t  loopCount = 0;
float magX;
float magY;
float magZ;
int magCount;

QueueHandle_t xQueue;

SemaphoreHandle_t sem_r, sem_w, sem;
  portBASE_TYPE s1, s2;
static TaskHandle_t xTaskToNotify = NULL;


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
  
static void ReadValues(void* arg){
  //read magnetometer values
  while(1){

    xSemaphoreTake(sem_r, portMAX_DELAY);
    
    sensor.readMagData();
    xSemaphoreGive(sem_w);
  }
}

/* 
 *  The reading of mag sesor give the count, in unit of 0.1 uT
 *  1T = 10^4 Gauss, 0.1x10**-6 * 10**4  = 10** -3 ie. 1 milliGauss
 *  Earth mag field is  ~0.5Gause == 500 mG 
 */
 
#define SCALE 1.0   /* in mG */
//#define SCALE 1.0E-3  /* in Gauss */
static void PrintMagValues()
{
  //print magnetometer values to serial port
     magX = sensor.magData.x*SCALE;
     magY = sensor.magData.y*SCALE;
     magZ = sensor.magData.z*SCALE;

    SerialUSB.println("Magnetometer X: ");
    SerialUSB.println(magX, 4);
    SerialUSB.println("Magnetometer Y: ");
    SerialUSB.println(magY, 4);
    SerialUSB.println("Magnetometer Z: ");
    SerialUSB.println(magZ, 4);
}

static void PrintValues(void* arg){
  //print magnetometer values to serial port
  while(1){
    xSemaphoreTake(sem_w, portMAX_DELAY);
    PrintMagValues();
    xSemaphoreGive(sem_r);
  }
}
void ISR1 (){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    noInterrupts();
    int gotIsR = 1;
    /* Notify the task to collect data */
/* 
 *  BaseType_t xQueueSendFromISR
           (
               QueueHandle_t xQueue,
               const void *pvItemToQueue,
               BaseType_t *pxHigherPriorityTaskWoken
           );
 */
    xQueueSendFromISR(xQueue,&gotIsR,&xHigherPriorityTaskWoken);
 //   vTaskNotifyGiveFromISR( xTaskToNotify, &xHigherPriorityTaskWoken );

    /* There are no transmissions in progress, so no tasks to notify. */
    xTaskToNotify = NULL;

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

    interrupts();
  
}
int buff[10] ={0};

static void CollectData(void *arg){
  static uint32_t thread_notification;
      /* Store the handle of the calling task. */

  SerialUSB.println("Task Created");
  while(1){

 //   thread_notification = ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
 if ( xQueueReceive( xQueue, &(buff), portMAX_DELAY ) ){
   //          SerialUSB.println("buff : "); 
 //            SerialUSB.println(buff[0], 4);
       
    sensor.readMagData();

    magX = sensor.magData.x*SCALE;
    magY = sensor.magData.y*SCALE;
    magZ = sensor.magData.z*SCALE;
    
    SerialUSB.print("Magnetometer X: ");
    SerialUSB.println(magX, 4);
    SerialUSB.print("Magnetometer Y: ");
    SerialUSB.println(magY, 4);
    SerialUSB.print("Magnetometer Z: ");
    SerialUSB.println(magZ, 4);

    ProcessData();
  }
 
} // end task CollectData()
}

void ProcessData(){
  
   magCount++;
  SerialUSB.println("Number of Data collections:  ");
  SerialUSB.println(magCount);
   
}

void setup() {
  int countMax=10000;

  uint16_t mean[3];
  uint16_t sd[3];
  uint16_t mag_threshold[3];
  const float coeff = 2.0;
  
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

  sensor.readMagData();
      SerialUSB.println(sensor.magData.x);
      SerialUSB.println(sensor.magData.y);
      SerialUSB.println(sensor.magData.z);

  sensor.readMagData();
      SerialUSB.println(sensor.magData.x);
      SerialUSB.println(sensor.magData.y);
      SerialUSB.println(sensor.magData.z);

  sensor.readMagData();
      SerialUSB.println(sensor.magData.x);
      SerialUSB.println(sensor.magData.y);
      SerialUSB.println(sensor.magData.z);

  sensor.Calibration( mean, sd);
  mag_threshold[0] =  mean[0] + 3.0 * sd[0];
  mag_threshold[1] =  mean[1] + 3.0 * sd[1];
  mag_threshold[2] =  mean[2] + 3.0 * sd[2];
  
  unsigned char debounce_time = 10;
  sensor.writeReg(FXOS8700CQ_A_VECM_CNT, debounce_time); //set debounce timer period to 100ms.

#if USE_SINGLE_X_MAG
  unsigned char  mag_config_val = 0;
  sensor.writeReg(FXOS8700CQ_M_VECM_CFG, mag_config_val); //set magnetic vector configuration. 0x69
  uint8_t mag_ths_config = B01001011;
  sensor.writeReg(FXOS8700CQ_M_THS_CFG, mag_ths_config); //disable magnetic-threshold interrupt. 0x52
  uint16_t mag_thresh = mag_threshold[0]; //1000; //1000 mg = 1 gauss
  SerialUSB.println("threshold: ");
  SerialUSB.println(mag_thresh);
  uint8_t mag_ths_msb = 0;
  uint8_t mag_ths_lsb = 0;
     mag_ths_msb = (mag_thresh >> 8) &0xFF;
     mag_ths_lsb = mag_thresh& 0xFF;
  SerialUSB.println("at threshold stage");
  sensor.writeReg(FXOS8700CQ_M_THS_X_MSB, mag_ths_msb); // MSB
  sensor.writeReg(FXOS8700CQ_M_THS_X_LSB, mag_ths_lsb); //LSB

  SerialUSB.println("written threshold to register");

#else  // ALL AXIS MAGNITUDEthreshold
  unsigned char  mag_config_val = B00011011;
  sensor.writeReg(FXOS8700CQ_M_VECM_CFG, mag_config_val); //set magnetic vector configuration. 0x69
  uint8_t mag_ths_config = 0;
  sensor.writeReg(FXOS8700CQ_M_THS_CFG, mag_ths_config); //disable magnetic-threshold interrupt.
  //set magnetic field thresholds to 10 Gauss = 1000uT. Earth is about 0.5Gauss
  // the following is the bit pattern of 10000 = 10 * 1000 mguass
  uint8_t mag_ths_msb = B00100111;
  uint8_t mag_ths_lsb = B00010000;
// new selection

    uint16_t mag_thresh = 1000; //1000 mg = 1 gause
     mag_ths_msb = (mag_thresh >> 8) &0xFF;
     mag_ths_lsb = mag_thresh& 0xFF;

  sensor.writeReg(FXOS8700CQ_M_VECM_THS_MSB, mag_ths_msb); //msb.
  sensor.writeReg(FXOS8700CQ_M_VECM_THS_LSB, mag_ths_lsb); //lsb
#endif

  magCount =0;
 
  SerialUSB.println("before attach Interrupt");
  xQueue = xQueueCreate( 1, sizeof(  int ) );
  attachInterrupt(digitalPinToInterrupt(INT_PIN), ISR1, FALLING);  
  SerialUSB.println ("attached Interrupt");
 s1 = xTaskCreate(CollectData, NULL, configMINIMAL_STACK_SIZE, NULL, 1, NULL);
   vTaskStartScheduler();
 

SerialUSB.println("end of setup");

}

void loop() {
}
