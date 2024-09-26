#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <DHT.h>
#include <DHT_U.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#define USE_UI_CONTROL 0
#if USE_UI_CONTROL
#include <MD_UISwitch.h>
#endif
// Turn on debug statements to the serial output
#define DEBUG 0
#if DEBUG
#define PRINT(s, x) { Serial2.print(F(s)); Serial2.print(x); }
#define PRINTS(x) Serial2.print(F(x))
#define PRINTX(x) Serial2.println(x, HEX)
#else
#define PRINT(s, x)
#define PRINTS(x)
#define PRINTX(x)
#endif
//RTC_DS1307 rtc;
void Task1code( void *pvParameters );
void Task2code( void *pvParameters );
void Task3code( void *pvParameters );
void Task4code( void *pvParameters );
TaskHandle_t Task1; //khai báo 2 task và đặt tên cho task
TaskHandle_t Task2; 
TaskHandle_t Task3; 
TaskHandle_t Task4; 
SemaphoreHandle_t Sem_Handle;
float humi ;
float tempC ;
float tempF ;
int giay;
int phut;
int gio;
int ngay;
int thang;
int nam;
char buf[10];
  char buf1[10];
  char buf2[10];
  char buf3[10];
  char buf4[10];
  char buf5[10];
  char buf6[10];
  char buf7[10];
  char buf8[10];
int flag;
int counter = 0;
#define DHT11_PIN  13 // ESP32 pin GPIO21 connected to DHT11 sensor
DHT dht11(DHT11_PIN, DHT11);

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 5
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#if USE_UI_CONTROL
const uint8_t SPEED_IN = A5;
const uint8_t DIRECTION_SET = 8;  // change the effect
const uint8_t INVERT_SET = 9;     // change the invert
const uint8_t SPEED_DEADBAND = 5;
#endif // USE_UI_CONTROL
uint8_t scrollSpeed = 30;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 200; // in milliseconds
// Global message buffers shared by Serial and Scrolling functions
#define    BUF_SIZE    200
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello, Nhap New Command" };
char dataMessage[BUF_SIZE] = { "" };
char data1Message[BUF_SIZE] = { "" };
bool newMessageAvailable = true;
#if USE_UI_CONTROL
MD_UISwitch_Digital uiDirection(DIRECTION_SET);
MD_UISwitch_Digital uiInvert(INVERT_SET);
void doUI(void)
{
  // set the speed if it has changed
  {
    int16_t speed = map(analogRead(SPEED_IN), 0, 1023, 10, 150);
    if ((speed >= ((int16_t)P.getSpeed() + SPEED_DEADBAND)) ||
      (speed <= ((int16_t)P.getSpeed() - SPEED_DEADBAND)))
    {
      P.setSpeed(speed);
      scrollSpeed = speed;
      PRINT("\nChanged speed to ", P.getSpeed());
    }
  }
  if (uiDirection.read() == MD_UISwitch::KEY_PRESS) // SCROLL DIRECTION
  {
    PRINTS("\nChanging scroll direction");
    scrollEffect = (scrollEffect == PA_SCROLL_LEFT ? PA_SCROLL_RIGHT : PA_SCROLL_LEFT);
    P.setTextEffect(scrollEffect, scrollEffect);
    P.displayClear();
    P.displayReset();
  }
  if (uiInvert.read() == MD_UISwitch::KEY_PRESS)  // INVERT MODE
  {
    PRINTS("\nChanging invert mode");
    P.setInvert(!P.getInvert());
  }
}
#endif // USE_UI_CONTROL

void dismatrix(void){
  if(flag==1||flag==3){   
        strcpy(dataMessage,"Time: ");
        strcat(dataMessage,buf3);
        strcat(dataMessage,":");
        strcat(dataMessage,buf4);
        strcat(dataMessage,":");
        strcat(dataMessage,buf5);
        strcat(dataMessage,"   ");
        strcat(dataMessage,"Thang: ");
        strcat(dataMessage,buf6);
        strcat(dataMessage,":");
        strcat(dataMessage,buf7);
        strcat(dataMessage,":");
        strcat(dataMessage,buf8);
        strcat(dataMessage,"   ");
  }
  if(flag==2|| flag==3){
      if(flag==3) 
        strcat(dataMessage,"nhietdo: ");
      else        
        strcpy(dataMessage,"nhietdo: ");
        strcat(dataMessage,buf);
        strcat(dataMessage,"%");
        strcat(dataMessage,"  ");
        strcat(dataMessage,buf1);
        strcat(dataMessage,"oC");
        strcat(dataMessage,"  ");
        strcat(dataMessage,buf2);
        strcat(dataMessage,"oF");
        strcat(dataMessage,"   ");

  }
   if(flag==1||flag==2) {
     strcat(dataMessage,data1Message+1);
     strcpy(newMessage,dataMessage);
     
   }
  if(flag==3){
  strcat(dataMessage,data1Message+2);
  strcpy(newMessage,dataMessage);
  }

}


void readSerial(void)
{ 
  int rlen;
  char temp[BUF_SIZE];
   
  while (Serial2.available() > 0) {
    // read the incoming bytes:
    rlen = Serial2.readBytes(newMessage,BUF_SIZE); 
    
  }
    if(rlen>0)
    {
      while(newMessage[rlen]!=0){
      newMessage[rlen]=0;
       rlen=rlen+1;      
       }
      newMessageAvailable=true;
      strcpy(data1Message,newMessage);
    }
    rlen=0;
    flag=0;
    if (newMessage[0]=='&'){
       if (newMessage[1]=='@')
             flag=3;
       else   flag=1;
    }
    if (newMessage[0]=='@')
             flag=2;

   Serial.print("I received: ");
   Serial.println(newMessage);   
}
void DelayMS(unsigned t)
{
    TickType_t currentTick = xTaskGetTickCount();
    while((xTaskGetTickCount()) - (currentTick) < pdMS_TO_TICKS(t));
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
} 

void readDS1307()
{
  tmElements_t tm;
  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
  }

  gio=tm.Hour;
  phut=tm.Minute;
  giay=tm.Second;
  ngay=tm.Day;
  thang = tm.Month;
  nam=tmYearToCalendar(tm.Year);
  itoa(gio, buf3, 10);
  itoa(phut, buf4, 10);
  itoa(giay, buf5, 10);
  itoa(ngay, buf6, 10);
  itoa(thang, buf7, 10);
  itoa(nam, buf8, 10);
  DelayMS(200);
}

void DHT_11(){
  humi  = dht11.readHumidity();
  tempC = dht11.readTemperature();
  tempF = dht11.readTemperature(true);

  // check whether the reading is successful or not
  if ( isnan(tempC) || isnan(tempF) || isnan(humi)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");
    Serial.print("  |  ");
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C  ~  ");
    Serial.print(tempF);
    Serial.println("°F");
  }
 
  dtostrf(humi,3,2,buf);
  dtostrf(tempC,3,2,buf1);
  dtostrf(tempF,3,2,buf2);
  DelayMS(200);
}
void Task1code(void *pvPara){
  while(1){
    DHT_11();
    xSemaphoreGive(Sem_Handle);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void Task2code(void *pvPara){
  while(1){
   #if USE_UI_CONTROL
  doUI();
   #endif // USE_UI_CONTROL
  if (P.displayAnimate())
  {
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = true;
    }
    P.displayReset();
  }
  dismatrix(); 
  }

}
void Task3code(void *pvPara){
  while(1){
   readDS1307();
   xSemaphoreTake(Sem_Handle,portMAX_DELAY);
  }
}
void Task4code(void *pvPara){
  while(1){
  readSerial();
  vTaskDelay(pdMS_TO_TICKS(1000));
  }
}
void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  dht11.begin(); 
#if USE_UI_CONTROL
  uiDirection.begin();
  uiInvert.begin();
  pinMode(SPEED_IN, INPUT);
  doUI();
#endif // USE_UI_CONTROL
  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
  Sem_Handle = xSemaphoreCreateBinary();
  xTaskCreate(Task1code,"Task1",8000,NULL,1,&Task1); 
  xTaskCreate(Task2code,"Task2",8000,NULL,1,&Task2); 
  xTaskCreate(Task3code,"Task3",8000,NULL,2,&Task3);  
  xTaskCreate(Task4code,"Task4",8000,NULL,1,&Task4);  
}
void loop() {
}