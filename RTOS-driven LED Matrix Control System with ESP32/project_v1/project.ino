#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <DHT.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define DHT11_PIN  13 
DHT dht11(DHT11_PIN, DHT11);
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 5
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

void Task1code( void *pvParameters );
void Task2code( void *pvParameters );
void Task3code( void *pvParameters );
void Task4code( void *pvParameters );
TimerHandle_t Task1;
TaskHandle_t Task2; 
TaskHandle_t Task3; 
SemaphoreHandle_t Sem_Handle;
int  curFX = 0;
float humi,tempC,tempF;
int giay,phut,gio,ngay,thang,nam,flag,flag1=0;
char buf[10], buf1[10],buf2[10],buf3[10],buf4[10],buf5[10],buf6[10],buf7[10],buf8[10];
int counter = 0;

// set ledmatrix........................................
uint8_t scrollSpeed = 20;    // default frame delay value
uint16_t scrollPause = 0; // in milliseconds
// Global message buffers shared by Serial and Scrolling functions............................
#define    BUF_SIZE    200
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello, Nhap New Command" };
char dataMessage[BUF_SIZE] = { "" };
char data1Message[BUF_SIZE] = { "" };
bool newMessageAvailable = true;
const uint16_t PAUSE_TIME = 500;



//Icon display ..................................................
const uint8_t F_PMAN1 = 6;
const  uint8_t W_PMAN1 = 8;
const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] =  // gobbling  pacman animation
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7a, 0x7e, 0x3c,
  0x00,  0x42, 0xe7, 0xe7, 0xff, 0xfb, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xfb,  0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xfb, 0x7e, 0x3c,
  0x24, 0x66,  0xe7, 0xff, 0xff, 0xfb, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xfb, 0x7e,  0x3c,
};


const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
const  uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] =  // ghost pursued by a pacman
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7a, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b,  0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xfb, 0x7e, 0x3c,  0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7,  0xff, 0xff, 0xfb, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73,  0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xfb, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe,  0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xfb, 0x7e,  0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42,  0xe7, 0xe7, 0xff, 0xfb, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb,  0x73, 0xfe,
};

const uint8_t F_ROCKET = 2;
const uint8_t W_ROCKET =  11;
const uint8_t PROGMEM rocket[F_ROCKET * W_ROCKET] =  // rocket
{
  0x18,  0x24, 0x42, 0x81, 0x99, 0x18, 0x99, 0x18, 0xa5, 0x5a, 0x81,
  0x18, 0x24, 0x42,  0x81, 0x18, 0x99, 0x18, 0x99, 0x24, 0x42, 0x99,
};

const uint8_t F_WALKER  = 5;
const uint8_t W_WALKER = 7;
const uint8_t PROGMEM walker[F_WALKER * W_WALKER]  =  // walking man
{
    0x00, 0x48, 0x77, 0x1f, 0x1c, 0x94, 0x68,
    0x00,  0x90, 0xee, 0x3e, 0x38, 0x28, 0xd0,
    0x00, 0x00, 0xae, 0xfe, 0x38, 0x28, 0x40,
    0x00, 0x00, 0x2e, 0xbe, 0xf8, 0x00, 0x00, 
    0x00, 0x10, 0x6e, 0x3e, 0xb8,  0xe8, 0x00,
};

const uint8_t F_HEART = 5;
const uint8_t W_HEART = 9;
const  uint8_t PROGMEM heart[F_HEART * W_HEART] =  // beating heart
{
  0x0e, 0x11,  0x21, 0x42, 0x84, 0x42, 0x21, 0x11, 0x0e,
  0x0e, 0x1f, 0x33, 0x66, 0xcc, 0x66,  0x33, 0x1f, 0x0e,
  0x0e, 0x1f, 0x3f, 0x7e, 0xfc, 0x7e, 0x3f, 0x1f, 0x0e,
  0x0e, 0x1f, 0x33, 0x66, 0xcc, 0x66, 0x33, 0x1f, 0x0e,
  0x0e, 0x11, 0x21, 0x42,  0x84, 0x42, 0x21, 0x11, 0x0e,
};

// .........................................................................
void dismatrix(void){
  if(flag==1||flag==3){   
        strcpy(dataMessage,"TIME: ");
        strcat(dataMessage,buf3);
        strcat(dataMessage,":");
        strcat(dataMessage,buf4);
        strcat(dataMessage,":");
        strcat(dataMessage,buf5);
        strcat(dataMessage,"   ");
        strcat(dataMessage,"DATE: ");
        strcat(dataMessage,buf6);
        strcat(dataMessage,"/");
        strcat(dataMessage,buf7);
        strcat(dataMessage,"/");
        strcat(dataMessage,buf8);
        strcat(dataMessage,"   ");
  }
  if(flag==2|| flag==3){
      if(flag==3) 
        strcat(dataMessage,"HUMI and TEMP: ");
      else        
        strcpy(dataMessage,"HUMI and TEMP: ");
        strcat(dataMessage,buf);
        strcat(dataMessage,"%");
        strcat(dataMessage,"  ");
        strcat(dataMessage,buf1);
        strcat(dataMessage,"oC");
        strcat(dataMessage,"  ");
        strcat(dataMessage,buf2);
        strcat(dataMessage,"oF");
        strcat(dataMessage,"  ");

  }
   if(flag==1||flag==2||flag==3) {
     strcat(dataMessage,data1Message);
     strcpy(newMessage,dataMessage);   
   }
}

//.................................................
void readSerial(void)
{ 
  int rlen;
  while (Serial2.available() > 0) {
    rlen = Serial2.readBytes(newMessage,BUF_SIZE); 
  }
  if(rlen>0)
    {
      while(newMessage[rlen]!=0){
        newMessage[rlen]=0;
        rlen=rlen+1;      
        }
      newMessageAvailable=true;
      strcpy(data1Message,newMessage+2);
    }
  rlen=0;
  flag=0;
  if (newMessage[0]=='@')
      flag=1;
  else if (newMessage[0]=='#')
      flag=2;
  else if (newMessage[0]=='$')
      flag=3;
  else if (newMessage[0]=='%')
      flag=4;
  if(newMessage[1]=='!') flag1=0;
  else if(newMessage[1]=='^') flag1=1;
  strcpy(newMessage,data1Message);
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
  gio=tm.Hour; phut=tm.Minute; 
  giay=tm.Second; ngay=tm.Day; 
  thang = tm.Month; nam=tmYearToCalendar(tm.Year);
  itoa(gio, buf3, 10); itoa(phut, buf4, 10); 
  itoa(giay, buf5, 10);itoa(ngay, buf6, 10);
  itoa(thang, buf7, 10); itoa(nam, buf8, 10);
  DelayMS(200);
}
void DHT_11(){
  humi  = dht11.readHumidity();
  tempC = dht11.readTemperature();
  tempF = dht11.readTemperature(true);
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
void icon()
{
  switch(curFX)
    {
      case 0:
        P.displayText(curMessage, PA_CENTER, 10, PAUSE_TIME,  PA_SPRITE, PA_SPRITE);
        P.setSpriteData(rocket, W_ROCKET, F_ROCKET,  rocket, W_ROCKET, F_ROCKET);
        break;
      case 1:
        P.displayScroll(curMessage, PA_LEFT, PA_SCROLL_LEFT,  300);

        break;
      case 2:
        P.displayScroll(curMessage, PA_RIGHT, PA_SCROLL_RIGHT, 300);
       
        break;
      case 3:
        P.displayText(curMessage, PA_CENTER, 10, PAUSE_TIME, PA_SPRITE, PA_SPRITE);
        P.setSpriteData(walker, W_WALKER, F_WALKER, walker, W_WALKER, F_WALKER);
        break;
      case 4:        
        P.displayScroll(curMessage, PA_LEFT, PA_SCROLL_LEFT, 300);        
     
        break;
      case 5:
        P.displayText(curMessage, PA_CENTER, 10, PAUSE_TIME,  PA_SPRITE, PA_SPRITE);
        P.setSpriteData(pacman1, W_PMAN1, F_PMAN1,  pacman2, W_PMAN2, F_PMAN2);
        break;
      case 6:        
        P.displayText(curMessage, PA_CENTER, 10, PAUSE_TIME,  PA_SPRITE, PA_SPRITE);
        P.setSpriteData(heart, W_HEART, F_HEART,  rocket, W_HEART, F_HEART);
        break;
    }
  curFX++;
  if (curFX > 6) 
    curFX = 0;
}
void Task1code(void *pvPara){
  while(1){
    DHT_11();
    xSemaphoreGive(Sem_Handle);
  }
}
void Task2code(void *pvPara){
  while(1){
      if (P.displayAnimate())
        {   
          if (newMessageAvailable)
            {
              strcpy(curMessage, newMessage);
              newMessageAvailable = true;
            }
          if(flag == 4)
            { 
              strcpy(curMessage, "");
              icon(); 
            }
          else
          if(flag1 == 0)
            P.displayText(curMessage, PA_LEFT, scrollSpeed, scrollPause, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          else 
            P.displayText(curMessage, PA_RIGHT, scrollSpeed, scrollPause, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
          P.displayReset();   
      }
        if (Serial2.available() > 0)
            readSerial();
        if(flag == 4)   DelayMS(30); 
      dismatrix(); 
  }
}
void Task3code(void *pvPara){
  while(1){
  xSemaphoreTake(Sem_Handle,portMAX_DELAY);
  xSemaphoreTake(Sem_Handle,portMAX_DELAY);
  xSemaphoreTake(Sem_Handle,portMAX_DELAY);
  readDS1307();
  }
}

void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  dht11.begin(); 
  P.begin();
  P.displayText(curMessage, PA_LEFT, scrollSpeed, scrollPause, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
  Sem_Handle = xSemaphoreCreateCounting(10, 0);
  Task1 = xTimerCreate("Task 1", pdMS_TO_TICKS(2000), pdTRUE, 0, Task1code);
  xTimerStart(Task1, portMAX_DELAY);
  xTaskCreatePinnedToCore(Task2code,"Task2",8000,NULL,1,&Task2,1);
  xTaskCreatePinnedToCore(Task3code,"Task3",8000,NULL,1,&Task3,1);  
}
void loop() {
}