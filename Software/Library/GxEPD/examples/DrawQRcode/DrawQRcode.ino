#include <GxEPD.h>
#include <GxGDE0213B1/GxGDE0213B1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/Roboto_Medium12pt7b.h>
#include <Fonts/Roboto_Medium8pt7b.h>
#include "qrcode.h"

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
  Serial.println("setup done");


  QRCode qrcode;
  uint8_t qrcode_size = 5;
  uint8_t qrcodeData[qrcode_getBufferSize(qrcode_size)];
  String QR_code_string = "Testing String For QR Code Demo";
  Serial.print("QR code data:"); Serial.println(QR_code_string);
  qrcode_initText(&qrcode, qrcodeData, qrcode_size, 0, QR_code_string.c_str());
  //display.println("Address: ");

  int x0 = 0;
  int y0 = 0;
  int expension = 2;
  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        display.fillRect(x0 + x * expension, y0 + y * expension, expension, expension, GxEPD_BLACK);
      } else {
        display.fillRect(x0 + x * expension, y0 + y * expension, expension, expension, GxEPD_WHITE);
      }
    }
  }
  display.update();
}

void loop()
{
  delay(10000);
}

