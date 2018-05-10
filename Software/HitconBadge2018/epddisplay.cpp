#include "epddisplay.hpp"
#include <GxEPD.h>
#include <GxGDE0213B1/GxGDE0213B1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/Roboto_Medium12pt7b.h>
#include <Fonts/Roboto_Medium8pt7b.h>

#include "control.hpp"
//QR code generator
#include "qrcode.h"
#include "icon.h"
#include "wallet.hpp"
#include "util.hpp"
#include "LFlash.h"

GxIO_Class io(SPI, 10, 7, 5);
GxGDE0213B1 display(io,5,4);

void init_display(){

  display.init();
  Serial.println("setup done");
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(3);
  // draw background

  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, true);
  delay(1000);
  display.fillScreen(GxEPD_WHITE);
  display.update();
  delay(1000);
}
//Battery_voltage ... etc
void display_static(){
  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  display.drawBitmap(wifi_full, 0, 2, 20, 15, GxEPD_BLACK);
  display.drawBitmap(BT, 25, 0, 20, 20, GxEPD_BLACK);
  display.drawBitmap(NFC, 50, 0, 20, 20, GxEPD_BLACK);
}


void main_menu(){
  display.setFont(&Roboto_Medium12pt7b);

  display.fillScreen(GxEPD_WHITE);
  display.setCursor(10, 50);
  display.println("Balance:");
  display.setCursor(10, 80);
  display.println("1.75 ETH");

  display.setFont(&Roboto_Medium8pt7b);
  
  display.setCursor(0,122-7);
  display.println("[A]:SET");
 
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(3)];
  qrcode_initText(&qrcode, qrcodeData, 3, 0, publicAddress_string.c_str());

  int x0 = 140;
  int y0 = 25;
  int expension = 3;
  for (uint8_t y = 0; y < qrcode.size; y++) {
      for (uint8_t x = 0; x < qrcode.size; x++) {
          if (qrcode_getModule(&qrcode, x, y)) {
            display.fillRect(x0+x*expension,y0+y*expension,expension,expension,GxEPD_BLACK);
          }else{
            display.fillRect(x0+x*expension,y0+y*expension,expension,expension,GxEPD_WHITE);
          }
      }
  }
  display_static();
  display.update();
  
}
void setting_menu(){
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&Roboto_Medium8pt7b);
  
  display.setCursor(50, 40);
  display.print("NFC: ");display.print("ON");
  display.println("   [4]:on,[5]:off");
  display.setCursor(50, 60);
  display.print("BLE: ");display.print("ON");
  display.println("   [7]:on,[8]:off");
  display.setCursor(50, 75);
  display.print("[B]:ReGenerateCode");
  
  display.setFont(&Roboto_Medium8pt7b);
  
  display.setCursor(0,122-7);
  display.println("[A]:Menu ");
 
  display_static();
  display.update();
  
  while(1){
      switch(readButton()){
        case 0xA: return;
        case 0xB:{
            BLE_pairing();
            return;
        }
        default:
            continue;
      }
  }

}

void BLE_pairing(){
  display.fillScreen(GxEPD_WHITE);
  display_static();
  display.setCursor(10, 60);
  uint32_t pin_code = randomNumbergenerator(6);
  display.setFont(&Roboto_Medium12pt7b);
  display.print("Piring Code:");display.println(pin_code);
  display.setCursor(0,122-7);
  display.setFont(&Roboto_Medium8pt7b);
  display.println("[A]:Return");
  display.update();

  LFlash.write("Wallet","BLE_paircode",LFLASH_RAW_DATA,(const uint8_t *)&pin_code,sizeof(pin_code));

  while(1){
    if(readButton()==0xA){
      return;
    }
  }

}
