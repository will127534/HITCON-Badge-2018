#include "epddisplay.hpp"
#include <GxEPD.h>
#include <GxGDEW027W3/GxGDEW027W3.cpp> 
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




GxIO_Class io(SPI, 10, A2, A1);
// GxGDEP015OC1(GxIO& io, uint8_t rst = 9, uint8_t busy = 7);
GxGDEW027W3 display(io, A1, A0);

void init_display(){

  display.init();
  Serial.println("setup done");
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(1);
  // draw background
  display.fillScreen(GxEPD_WHITE);
  display.update();
  //delay(1000);
}
//Battery_voltage ... etc
void display_static(){
  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(0.001);
  display.println(" V");

  
  //display.drawBitmap(wifi_full, 0, 2, 20, 15, GxEPD_BLACK);
  //display.drawBitmap(BT, 25, 0, 20, 20, GxEPD_BLACK);
  //display.drawBitmap(NFC, 50, 0, 20, 20, GxEPD_BLACK);
}


void main_menu(){
  display.setFont(&Roboto_Medium12pt7b);

  display.fillScreen(GxEPD_WHITE);
  display.setCursor(30, 50);
  display.println("Balance:");
  display.setCursor(30, 80);

  double Balance;
  uint32_t size = 8;
  LFlashStatus status = LFlash.read("Wallet","Balance",(uint8_t *)&Balance,&size);

  if (status == LFLASH_ITEM_NOT_FOUND)
  {
    Balance = 0;
    LFlash.write("Wallet","Balance",LFLASH_RAW_DATA,(const uint8_t *)&Balance,sizeof(Balance));
  }

  display.setFont(&Roboto_Medium8pt7b);
  display.print(Balance,4);display.print(" ETH");
  
  display.setCursor(0,122-7);
  display.print("[A]:SET ");
 
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(4)];
  String QR_code_string = String("hitcon://tran?a=")+publicAddress_string;
  Serial.println(QR_code_string);
  qrcode_initText(&qrcode, qrcodeData, 4, 0, QR_code_string.c_str());

  display.setCursor(150,40);
  display.println("Address: ");

  int x0 = 150;
  int y0 = 50;
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
  display_static();
  display.update();
  //For partial update ==> update background using partial update irst
  
}
void setting_menu(){
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&Roboto_Medium8pt7b);

  display.setCursor(50, 75);
  display.print("[1]:ReParing BLE");
  
  display.setFont(&Roboto_Medium8pt7b);
  
  display.setCursor(0,122-7);
  display.println("[A]:Menu");
  
  display_static();
  display.update();
  

  while(1){
  if(Serial.available()){
    char c = Serial.read();
    if(c=='1'){
        BLE_pairing();
        return;   
    }
  }
  }

}

void BLE_pairing(){
  bool changed = 1;
  LFlash.write("BLE","NewSetting",LFLASH_RAW_DATA,(const uint8_t *)&changed,sizeof(changed));
  WDT_Reset();
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

    uint8_t AES_key[16] = {0};
    size = sizeof(AES_key);
    LFlash.read("BLE","BLE_AES_key",(uint8_t *)&AES_key,&size);

    char AES_buf[32+1];
    char *pos = AES_buf;

    for (int i = 0 ; i < 16 ; i++) {
        pos += sprintf(pos, "%02x", AES_key[i]);
    }
    Serial.println(AES_buf);


    sprintf (AES_key_buffer, "hitcon://pair?v=18&a=%s&k=%0s&s=%s&c=%s%s%s%s%s%s",publicAddress_string.c_str(), AES_buf,ServiceUUID,UUIDHead[0],UUIDHead[1],UUIDHead[2],UUIDHead[3],UUIDHead[4],UUIDHead[5]);

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
    display_static();
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
