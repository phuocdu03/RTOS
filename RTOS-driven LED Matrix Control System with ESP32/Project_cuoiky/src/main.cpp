#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <DHT.h>
#include <DHT_U.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

//RTC_DS1307 rtc;
void Task1code( void *pvParameters );
void Task2code( void *pvParameters );
void Task3code( void *pvParameters );
TaskHandle_t Task1; //khai báo 2 task và đặt tên cho task
TaskHandle_t Task2; 
TaskHandle_t Task3; 

#define DHT11_PIN  13 // ESP32 pin GPIO21 connected to DHT11 sensor
DHT dht11(DHT11_PIN, DHT11);

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 5
MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

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
  DelayMS(200);
}



void MATRIX() {
  Display.setInvert(true);
  Display.setTextAlignment(PA_LEFT);
  Display.print("DU"); // display text
   DelayMS(200);
  Display.setTextAlignment(PA_CENTER);
  Display.print("PHAT"); // display text
 DelayMS(200);
  Display.setTextAlignment(PA_RIGHT);
  Display.print("NHAT"); // display text
 DelayMS(200);
  Display.setTextAlignment(PA_CENTER);
  Display.print("DANG"); // display text
   DelayMS(200);
  Display.setTextAlignment(PA_CENTER);
  Display.print("TEAM"); // display text inverted
   DelayMS(200);
}


void DHT_11(){
  float humi  = dht11.readHumidity();
  float tempC = dht11.readTemperature();
  float tempF = dht11.readTemperature(true);

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
       DelayMS(200);
  }
  
}
void Task1code(void *pvPara){
  while(1){
    DHT_11();
    DelayMS(200);
  }
}
void Task2code(void *pvPara){
  while(1){
    MATRIX();
    Serial.println("Task2 is running");
  }
}
void Task3code(void *pvPara){
  while(1){
  readDS1307();
  }
}
void setup() {
  Wire.begin();
  Serial.begin(9600);
  dht11.begin(); 
  Display.begin();
  Display.setIntensity(0);
  Display.displayClear(); 

  xTaskCreatePinnedToCore(Task1code,"Task1",16384,NULL,1,&Task1,0); 
  xTaskCreatePinnedToCore(Task2code,"Task2",16384,NULL,1,&Task2,1); 
  xTaskCreatePinnedToCore(Task3code,"Task3",16384,NULL,1,&Task3,0);  

}

void loop() {

}
 





