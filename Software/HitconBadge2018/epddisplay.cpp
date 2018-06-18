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
#include "BLE.hpp"
GxIO_Class io(SPI, 10, 7, 5);
GxGDE0213B1 display(io,5,4);

void init_display(){

  display.init();
  Serial.println("setup done");
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(3);
  // draw background

  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, true);

  //delay(1000);
  display.fillScreen(GxEPD_WHITE);
  display.update();
  //delay(1000);
}
//Battery_voltage ... etc
void display_static(){
  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  
  //display.drawBitmap(wifi_full, 0, 2, 20, 15, GxEPD_BLACK);
  //display.drawBitmap(BT, 25, 0, 20, 20, GxEPD_BLACK);
  //display.drawBitmap(NFC, 50, 0, 20, 20, GxEPD_BLACK);
}


void main_menu(){
  display.setFont(&Roboto_Medium12pt7b);

  display.fillScreen(GxEPD_WHITE);
  display.setCursor(10, 50);
  display.println("Balance:");
  display.setCursor(10, 80);

  double Balance;
  uint32_t size = 8;
  LFlashStatus status = LFlash.read("Wallet","Balance",(uint8_t *)&Balance,&size);

  if (status == LFLASH_ITEM_NOT_FOUND)
  {
    Balance = 0;
    LFlash.write("Wallet","Balance",LFLASH_RAW_DATA,(const uint8_t *)&Balance,sizeof(Balance));
  }

  display.setFont(&Roboto_Medium8pt7b);
  display.print(Balance,4);
  display.println(" ETH");
  
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
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  display.powerDown();
  //For partial update ==> update background using partial update irst
  
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

  display.setCursor(0,122-7);
  display.setFont(&Roboto_Medium8pt7b);
  display.println("[A]:Reset");
  display.update();
    
  bool changed = 1;
  LFlash.write("BLE","NewSetting",LFLASH_RAW_DATA,(const uint8_t *)&changed,sizeof(changed));

  while(1){
    if(readButton()==0xA){
     WDT_Reset();
    }
  }

}

void display_paringQR(){
    char AES_key_buffer[200] = {0};

    char ServiceUUID[37] = {0};
    uint32_t size = 37;

    LFlash.read("BLE","Service_UUID",(uint8_t *)&ServiceUUID,&size);

    size = 8;
    char UUIDHead[6][9] = {0};
    LFlash.read("BLE","Transaction",(uint8_t *)&UUIDHead[0],&size);
    LFlash.read("BLE","Txn",(uint8_t *)&UUIDHead[1],&size);
    LFlash.read("BLE","AddERC20",(uint8_t *)&UUIDHead[2],&size);
    LFlash.read("BLE","Balance",(uint8_t *)&UUIDHead[3],&size);
    LFlash.read("BLE","General_CMD",(uint8_t *)&UUIDHead[4],&size);
    LFlash.read("BLE","General_Data",(uint8_t *)&UUIDHead[5],&size);

    uint32_t AES_key[4] = {0};
    size = sizeof(AES_key);
    LFlash.read("BLE","BLE_AES_key",(uint8_t *)&AES_key,&size);

    sprintf (AES_key_buffer, "hitcon://pair?v=18&a=%s&k=%08x%08x%08x%08x&s=%s&c=%s%s%s%s%s%s",publicAddress_string.c_str(), AES_key[0], AES_key[1], AES_key[2], AES_key[3],ServiceUUID,UUIDHead[0],UUIDHead[1],UUIDHead[2],UUIDHead[3],UUIDHead[4],UUIDHead[5]);

    Serial.print("QR:");Serial.println(AES_key_buffer);
    Serial.println(qrcode_getBufferSize(8));
    
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(8)];
    qrcode_initText(&qrcode, qrcodeData, 8, ECC_LOW, AES_key_buffer);

    int x0 = 75;
    int y0 = 20;
    int expension = 2;
    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            if (qrcode_getModule(&qrcode, x, y)) {
              display.fillRect(x0+x*expension,y0+y*expension,expension,expension,GxEPD_BLACK);
            }else{
              display.fillRect(x0+x*expension,y0+y*expension,expension,expension,GxEPD_WHITE);
            }
        }
    }

    display.setCursor(25,9);
    display.setFont(&Roboto_Medium8pt7b);
    display.println("Waiting for connection.....");

    display.setCursor(0,122-7);
    display.setFont(&Roboto_Medium8pt7b);
    display.println("[A]:Menu");
    display.update();
    Serial.println("Waiting for connection");
    while(1){
      if(LBLEPeripheral.connected()>0){
        bool changed = 0;
        LFlash.write("BLE","NewSetting",LFLASH_RAW_DATA,(const uint8_t *)&changed,sizeof(changed));
        break;
      }
      Serial.print(".");
      delay(1000);
    }
}