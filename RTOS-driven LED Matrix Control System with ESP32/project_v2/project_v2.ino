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
TimerHandle_t Task1;
TaskHandle_t Task2; 
TaskHandle_t Task3; 
SemaphoreHandle_t Sem_Handle;
int  curFX = 0;
float humi,tempC,tempF;
int giay,phut,gio,ngay,thang,nam,flag=0,flag1=0;

// set ledmatrix........................................
uint8_t scrollSpeed = 30;    // default frame delay value
uint16_t scrollPause = 0; // in milliseconds
const uint16_t PAUSE_TIME = 500;

#define    BUF_SIZE    200
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello, Nhap New Command" };
String data = "Hello, Nhap New Command";    
String data_read = "";
bool check_serial = false;

int i = 0;
char *text[] ={"    ", "THIS", "IS", "GROUP", "5", "HAVE", "A", "NICE", "DAY"};
textEffect_t texteffect[] ={PA_PRINT,PA_MESH,PA_SCROLL_UP_LEFT,PA_SCROLL_DOWN_RIGHT,PA_BLINDS,PA_GROW_UP,PA_SCROLL_UP,PA_SCROLL_DOWN,PA_SCROLL_LEFT};

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
  String s = "";
  if(flag==1||flag==3)
  {   
    s = String("TIME: ") + gio + String(":") + phut + String(":") + giay + String("   ") + "DATE: " + ngay + String("/") + thang + String("/") + nam + String("   ");
  }
  if(flag==2)
    s = String("HUMI and TEMP: ") + humi + String("%") + String("   ") + tempC + String("oC   ") + tempF + String("oF   ");
  if(flag==3)
    s += String("HUMI and TEMP: ") + humi + String("%") + String("   ") + tempC + String("oC   ") + tempF + String("oF   ");
  s += data;
  s.toCharArray(newMessage, BUF_SIZE);
}

//.................................................
void serialEvent2() 
{
  while (Serial2.available()) 
  {
    char inChar = (char)Serial2.read();
    data_read += inChar;  
  }
  DelayMS(20);
  if(Serial2.available() == 0)
    check_serial = true;
}
void data_process()
{
  
  if(check_serial)
  {
    data = data_read;
    data_read = "";
    if (data[0]=='@')
        flag=1;
    else if (data[0]=='#')
        flag=2;
    else if (data[0]=='$')
        flag=3;
    else if (data[0]=='%')
        {
          flag=4;
          i = 0;
          curFX = 0;
        }
    else
        flag = 0;
    if(data[1]=='!')      flag1=0;
    else if(data[1]=='^') flag1=1;
    data.remove(0, 2);
    Serial.print("I received: ");
    Serial.println(data); 
    check_serial = false;
  }
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
  DelayMS(200);
}
void icon()
{
  switch(curFX)
    {
      case 0:
        P.displayText("", PA_CENTER, 10, PAUSE_TIME,  PA_SPRITE, PA_SPRITE);
        P.setSpriteData(rocket, W_ROCKET, F_ROCKET,  rocket, W_ROCKET, F_ROCKET);
        break;
      case 1:
        P.displayScroll("", PA_LEFT, PA_SCROLL_LEFT,  300);

        break;
      case 2:
        P.displayScroll("", PA_RIGHT, PA_SCROLL_RIGHT, 300);
       
        break;
      case 3:
        P.displayText("", PA_CENTER, 10, PAUSE_TIME, PA_SPRITE, PA_SPRITE);
        P.setSpriteData(walker, W_WALKER, F_WALKER, walker, W_WALKER, F_WALKER);
        break;
      case 4:        
        P.displayScroll("", PA_LEFT, PA_SCROLL_LEFT, 300);        
     
        break;
      case 5:
        P.displayText("", PA_CENTER, 10, PAUSE_TIME,  PA_SPRITE, PA_SPRITE);
        P.setSpriteData(pacman1, W_PMAN1, F_PMAN1,  pacman2, W_PMAN2, F_PMAN2);
        break;
      case 6:        
        P.displayText("", PA_CENTER, 10, PAUSE_TIME,  PA_SPRITE, PA_SPRITE);
        P.setSpriteData(heart, W_HEART, F_HEART,  walker, W_WALKER, F_WALKER);
        break;
      case 7:
        P.displayText(text[i], PA_CENTER, 50, 100, texteffect[i], texteffect[i]);
        i++;
        if(i<9)  
         curFX--;       
        else
        i=0;
        break;
    }
  curFX++;
  if (curFX > 7) 
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
          data_process();
          dismatrix(); 
          strcpy(curMessage, newMessage);
          if(flag == 4)
              icon(); 
          else
          if(flag1 == 0)
            P.displayText(curMessage, PA_LEFT, scrollSpeed, scrollPause, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
          else 
            P.displayText(curMessage, PA_RIGHT, scrollSpeed, scrollPause, PA_SCROLL_RIGHT, PA_SCROLL_RIGHT);
          P.displayReset();   
        }
      if(flag == 4)   DelayMS(30); 
      
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
