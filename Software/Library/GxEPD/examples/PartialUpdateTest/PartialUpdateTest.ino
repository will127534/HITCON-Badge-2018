#include <GxEPD.h>
#include <GxGDE0213B1/GxGDE0213B1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/Roboto_Medium12pt7b.h>
#include <Fonts/Roboto_Medium8pt7b.h>

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
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  display.powerDown();
  Serial.println("setup done");
}


void loop() {
  display.fillRect(0, 0, 59, 30, GxEPD_WHITE);
  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(0, 15);
  display.print(millis());
  display.println("ms");
  display.updateWindow(0, 0, 59, 15, true);
  delay(1000);
}