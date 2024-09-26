#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
// set to 1 if we are implementing the user interface pot, switch, etc
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
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    5
// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// SOFTWARE SPI
//MD_Parola P = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
// Scrolling parameters
#if USE_UI_CONTROL
const uint8_t SPEED_IN = A5;
const uint8_t DIRECTION_SET = 8;  // change the effect
const uint8_t INVERT_SET = 9;     // change the invert
const uint8_t SPEED_DEADBAND = 5;
#endif // USE_UI_CONTROL
uint8_t scrollSpeed = 25;    // default frame delay value
textEffect_t scrollEffect = PA_SCROLL_LEFT;
textPosition_t scrollAlign = PA_LEFT;
uint16_t scrollPause = 200; // in milliseconds
// Global message buffers shared by Serial and Scrolling functions
#define    BUF_SIZE    75
char curMessage[BUF_SIZE] = { "" };
char newMessage[BUF_SIZE] = { "Hello, Nhap New Command" };
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
void readSerial(void)
{ 
  int rlen;
 // static char *buf = newMessage;
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
    }
    rlen=0;
  Serial.println("I received: ");
  Serial.print(newMessage);   
  
}
void setup()
{
  Serial.begin(9600);
  Serial2.begin(9600);
 Serial.print("\n[Parola Scrolling Display]\nType a message for the scrolling display\nEnd message line with a newline");
#if USE_UI_CONTROL
  uiDirection.begin();
  uiInvert.begin();
  pinMode(SPEED_IN, INPUT);
  doUI();
#endif // USE_UI_CONTROL
  P.begin();
  P.displayText(curMessage, scrollAlign, scrollSpeed, scrollPause, scrollEffect, scrollEffect);
}
void loop()
{
#if USE_UI_CONTROL
  doUI();
#endif // USE_UI_CONTROL
  if (P.displayAnimate())
  {
    if (newMessageAvailable)
    {
      strcpy(curMessage, newMessage);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
  if(Serial2.available() > 0)
  readSerial();
}