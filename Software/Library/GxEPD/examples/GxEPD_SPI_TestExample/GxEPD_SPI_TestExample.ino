#include <GxEPD.h>
#include <GxGDE0213B1/GxGDE0213B1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/Roboto_Medium12pt7b.h>
#include <Fonts/Roboto_Medium8pt7b.h>
#include "icon.h"

GxIO_Class io(SPI, 10, 7, 5); //pinout for HitconBadge2018
GxGDE0213B1 display(io, 5, 4); //pinout for HitconBadge2018

void setup()
{
  Serial.begin(115200);
  Serial.println("setup");

  display.init();
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(3);
  display.fillScreen(GxEPD_WHITE);
  display.update();
  Serial.println("setup done");
}

void loop()
{
  showBitmapExample();
  delay(10000);
  drawText();
  delay(10000);
}

void showBitmapExample() {
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(HITCON, 30, 0, 193, 121, GxEPD_BLACK);
  display.update();
}
void drawText() {
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(0, 20);
  display.println(0.1234, 3);
  display.println("Hello World for Hitcon Badge 2018");
  display.update();
}
