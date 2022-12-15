#include <SPI.h>
#include <TFT_eSPI.h>       // Include the graphics library - install and configure https://github.com/Bodmer/TFT_eSPI
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"

// The font names are arrays references, thus must NOT be in quotes ""
#define AA_FONT_SMALL NotoSansBold15
#define AA_FONT_LARGE NotoSansBold36

// Settings
#define SOUND_SPEED 0.034 // sound speed in cm/uS
#define SCREEN_W 135
#define SCREEN_H 240
#define GPIO_TRIG 32
#define GPIO_ECHO 33
#define GPIO_BL 4
#define SLOW_DOWN 97
#define STOP_FROM 77 // inclusve
#define STOP_TO 73 // inclusve

TFT_eSPI tft = TFT_eSPI();  // Create object "tft"

long duration;
float distanceCm = 150;
float previousDistanceCm = 0;
bool triggerChange = false;
bool backlight = true;
int sleepCtr = 0;
int testCtr = 0;
int chevronOffset = 0;

void setup() {
  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);
  tft.loadFont(AA_FONT_LARGE);
  pinMode(GPIO_TRIG, OUTPUT);
  pinMode(GPIO_ECHO, INPUT);
}

void loop()
{
  checkDistance();
  //mockCheckDistance();

  if (sleepCtr > 250) {
    backlightOff();
  } else {
    if (triggerChange)
    {
      backlightOn();
      updateScreen();
    }
  }

  if (sleepCtr > 99999) {
    sleepCtr = 999;
  }
  sleepCtr++;

  delay(40);
}

void mockCheckDistance() {
  if (testCtr == 50) {
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
  else if (testCtr == 400)
  {
    distanceCm = 150;
    testCtr = 0;
  }
  testCtr++;
  distanceChanges();
}

void checkDistance()
{
  digitalWrite(GPIO_TRIG, LOW); // Clears the GPIO_TRIG
  delayMicroseconds(2);
  digitalWrite(GPIO_TRIG, HIGH); // Sets the GPIO_TRIG on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(GPIO_TRIG, LOW);
  duration = pulseIn(GPIO_ECHO, HIGH); // Reads the GPIO_ECHO, returns the sound wave travel time in microseconds
  distanceCm = duration * SOUND_SPEED / 2; // Calculate distance
  distanceChanges();
}

void distanceChanges() {
  triggerChange = abs(previousDistanceCm - distanceCm) >= 0.5;
  previousDistanceCm = distanceCm;
  if (triggerChange) {
    sleepCtr = 0;
  }
}

void updateScreen()
{  
  if (distanceCm > SLOW_DOWN)
  {
    tft.fillScreen(TFT_GREEN);
    tft.setTextColor(TFT_BLACK, TFT_GREEN, true);
  }
  else if (distanceCm <= SLOW_DOWN && distanceCm > STOP_FROM)
  {
    tft.fillScreen(TFT_ORANGE);
    tft.setTextColor(TFT_BLACK, TFT_ORANGE, true);
  }
  else if (distanceCm <= STOP_FROM && distanceCm >= STOP_TO)
  {
    tft.fillScreen(TFT_RED);
    tft.setTextColor(TFT_BLACK, TFT_RED, true);
  }
  else
  {
    tft.fillScreen(TFT_PURPLE);
    tft.setTextColor(TFT_WHITE, TFT_PURPLE, true);
  }

  tft.setCursor(10, 50);
  tft.print((int)distanceCm);

  //tft.fillScreen(TFT_BLACK);
  //tft.pushImage(0, 0 - chevronOffset, 135, 59 - chevronOffset, ChevronGreen);
  //tft.pushImage(0, 61 - chevronOffset, 135, 121 - chevronOffset, ChevronGreen02);
  //tft.pushImage(0, 119 - chevronOffset, 135, 179 - chevronOffset, ChevronGreen);
  //tft.pushImage(0, 240 - chevronOffset, 135, 300 - chevronOffset, ChevronGreen);
  // chevronOffset++;
  // if (chevronOffset > 60) chevronOffset = 0;

  // tft.fillScreen(TFT_BLACK);
  // tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  // tft.loadFont(AA_FONT_LARGE);
  // tft.setCursor(10, 50);
  // tft.print(distanceCm);

}

void backlightOn()
{
  if (!backlight) {
    digitalWrite(GPIO_BL, HIGH);
    backlight = true;
  }
}

void backlightOff()
{
  if (backlight) {
    digitalWrite(GPIO_BL, LOW);
    backlight = false;
  }
}

void drawArrow()
{
  tft.fillScreen(TFT_BLACK);
  int xMid = SCREEN_W / 2;
  for (int y = 0; y < SCREEN_H; y++)
  {
    if (y < xMid)
    {
      tft.drawLine(xMid - y, y, xMid + y, y, TFT_GREEN);
    }
    else
    {
      tft.drawLine(0, y, SCREEN_W - 1, y, TFT_GREEN);
    }
  }
}

void testScreenSize()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
  tft.loadFont(AA_FONT_LARGE);

  int x = 0;
  int y = 0;

  while (x < SCREEN_W || y < SCREEN_H) {
    if (x < SCREEN_W) {
      tft.setCursor(30, 50);
      tft.drawPixel(x, 0, TFT_WHITE);
      tft.print(++x);
    }
    if (y < SCREEN_H) {
      tft.setCursor(30, 150);
      tft.drawPixel(0, y, TFT_WHITE);
      tft.print(++y);
    }
    delay(10);
  }
  tft.unloadFont();
  delay(5000);
}
