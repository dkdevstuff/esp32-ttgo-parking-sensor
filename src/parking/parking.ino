#include <SPI.h>
#include <TFT_eSPI.h> // Include the graphics library - install and configure https://github.com/Bodmer/TFT_eSPI
#include "NotoSansBold15.h"
#include "ImgGo.h"
#include "ImgSlow.h"
#include "ImgStop.h"
#include "ImgBack.h"

// Settings
#define SOUND_SPEED 0.034 // sound speed in cm/uS
#define GPIO_TRIG 32
#define GPIO_ECHO 33
#define GPIO_BL 4
#define SLOW_DOWN 97
#define STOP_FROM 77 // inclusve
#define STOP_TO 73   // inclusve

TFT_eSPI tft = TFT_eSPI(); // Create object "tft"

long duration;
float distanceCm = 150;
float previousDistanceCm = 0;
bool triggerChange = false;
bool backlight = true;
int sleepCtr = 0;
int testCtr = 0;
int moveState = 1;

void setup()
{
  tft.init();
  tft.setRotation(2);
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_BLACK);
  tft.loadFont(NotoSansBold15);
  pinMode(GPIO_TRIG, OUTPUT);
  pinMode(GPIO_ECHO, INPUT);
}

void loop()
{
  checkDistance();
  // mockCheckDistance();
  distanceChanges();

  if (sleepCtr > 250)
  {
    backlightOff();
  }
  else
  {
    if (triggerChange)
    {
      backlightOn();
      updateScreen();
    }
  }

  if (sleepCtr > 99999)
  {
    sleepCtr = 999;
  }
  sleepCtr++;

  delay(40);
}

void mockCheckDistance()
{
  if (testCtr == 50)
  {
    distanceCm = SLOW_DOWN;
  }
  else if (testCtr == 100)
  {
    distanceCm = STOP_FROM;
  }
  else if (testCtr == 150)
  {
    distanceCm = STOP_TO - 5;
  }
  else if (testCtr == 600)
  {
    distanceCm = 150;
    testCtr = 0;
  }
  testCtr++;
}

void checkDistance()
{
  digitalWrite(GPIO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(GPIO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(GPIO_TRIG, LOW);
  duration = pulseIn(GPIO_ECHO, HIGH);     // Reads the GPIO_ECHO, returns the sound wave travel time in microseconds
  distanceCm = duration * SOUND_SPEED / 2; // Calculate distance
}

void distanceChanges()
{
  triggerChange = abs(previousDistanceCm - distanceCm) >= 0.5;
  previousDistanceCm = distanceCm;
  if (triggerChange)
  {
    sleepCtr = 0;
  }
}

void updateScreen()
{
  if (distanceCm > SLOW_DOWN)
  {
    tft.pushImage(0, 0, 135, 240, ImgGo);
  }
  else if (distanceCm <= SLOW_DOWN && distanceCm > STOP_FROM)
  {
    tft.pushImage(0, 0, 135, 240, ImgSlow);
  }
  else if (distanceCm <= STOP_FROM && distanceCm >= STOP_TO)
  {
    tft.pushImage(0, 0, 135, 240, ImgStop);
  }
  else
  {
    tft.pushImage(0, 0, 135, 240, ImgBack);
  }
  tft.setCursor(55, 220);
  tft.print((int)distanceCm);
}

void backlightOn()
{
  if (!backlight)
  {
    digitalWrite(GPIO_BL, HIGH);
    backlight = true;
  }
}

void backlightOff()
{
  if (backlight)
  {
    tft.fillScreen(TFT_BLACK);
    digitalWrite(GPIO_BL, LOW);
    backlight = false;
  }
}
