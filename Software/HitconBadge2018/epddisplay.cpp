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
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(3);
  // draw background
  //display.fillScreen(GxEPD_WHITE);
  //display.update();
  //display.updateWindow(0, 0, 250, 122, true);

  //delay(1000);
  display.fillScreen(GxEPD_WHITE);
  //display.update();
  //delay(1000);
}
//Battery_voltage ... etc
void display_static(){
  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  if(LBLEPeripheral.connected()>0){
        Serial.println("Connected");
        display.drawBitmap(BT, 0, 2, 20, 20, GxEPD_BLACK);
 }


  /*
     y0,x0
     /|
     \|
     y1,x0
  */


  uint16_t x0 = 10;
  uint16_t x1 = 240;

  uint16_t y0 = 56;
  uint16_t y1 = 66;

  display.fillTriangle(x0,y0,x0,y1,5,61, GxEPD_BLACK);
  display.setCursor(4, 51);
  display.println("4");

  display.fillTriangle(x1,y0,x1,y1,245,61, GxEPD_BLACK);
  display.setCursor(246-8, 51);
  display.println("6");


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
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  display.powerDown();
  //For partial update ==> update background using partial update irst
  
}
void setting_menu(){
  display.fillScreen(GxEPD_WHITE);
  display.setFont(&Roboto_Medium8pt7b);

  uint16_t isolation = 20;
  uint16_t text_x0 = 50;
  uint16_t text_y0 = 44;
  

  display.setCursor(text_x0, text_y0);
  display.print("2   BLE Re-Paring");

  display.setCursor(text_x0, text_y0+isolation*1);
  display.print("5   NFC ON");

  display.setCursor(text_x0, text_y0+isolation*2);
  display.print("8   Reinit Wallet");
  
  display_static();
  display.update();
  /*
  while(1){
    int button = readButton();
    Serial.println(button);
      switch(button){
        case 0xA: return;
        case 0x1:{
          Serial.println("Paring!");
            BLE_pairing();
            return;
        }
        default:
            continue;
      }
  }
  */

}

bool pincode_page(uint8_t *pincode){
  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  display.fillScreen(GxEPD_WHITE);

  display.setFont(&Roboto_Medium8pt7b);
  uint16_t x0 = 20;
  display.setCursor(x0, 50);
  display.println("Please Enter Pin Code:");
  display.drawBitmap(lock, 200, 45, 28, 41, GxEPD_BLACK);
  display.updateWindow(200, 45,28, 41, true);
  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  //display.powerDown();
  //display.powerDown();
  uint8_t pointer = 0;
  bool update = 0;
  bool hide_lastdigit = 0;
  uint32_t lastpin_time = 0;

  while(1){

    int button_pressed = readButton();
    if(button_pressed == 0x0D){
      if (pointer>0)
      {
        pointer --;
      }
      update = 1;
      hide_lastdigit = 1;
    }
    if(button_pressed == 0x0E){
      if (pointer<6)
      {
        continue;
      }
      display.drawBitmap(unlock_int, 200, 45, 28, 41, GxEPD_BLACK);
      display.updateWindow(200, 45,28, 41, true);

      display.drawBitmap(unlock, 200, 45, 28, 41, GxEPD_BLACK);
      display.updateWindow(200, 45,28, 41, true);

      display.setTextSize(0);
      return 1;
    }
    if (button_pressed<0x0A && pointer<6 && button_pressed != -1)
    {
      pincode[pointer] = button_pressed;
      Serial.println(button_pressed);
      pointer ++;
      update = 1;
      lastpin_time = millis();
      hide_lastdigit = 0;
    }

    if (millis() - lastpin_time > 2500 && hide_lastdigit ==0 && pointer != 0)
    {
      update = 1;
      hide_lastdigit = 1;
    }


    if (update)
    {
      digitalWrite(LED,HIGH);
      display.setTextSize(2);
      display.fillRect(x0+15, 85 - 30, 150, 35,GxEPD_WHITE);
      display.setCursor(x0+15, 85);
      if (pointer!=0)
      {
        for (int i = 0; i < pointer-1; ++i)
        {
          display.print(" *");
        }
        if (hide_lastdigit)
        {
          display.print(" *");
        }
        else{
          display.print(" ");
          display.print(pincode[pointer-1]);        
        }

      }
      display.updateWindow(x0+15, 85 - 30, 150, 35, true);
      digitalWrite(LED,LOW);
    }

      update = 0;
  }
  
}


bool transaction_page(transaction_struct trx_test){
  display.setFont(&Roboto_Medium8pt7b);
  display.setTextSize(0);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");


  String to = "0x" + vector2string(trx_test.to);

  display.fillScreen(GxEPD_WHITE);
  display.updateWindow(0, 0, 250, 122, false);

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(30, 20);
  display.println("New Tranaction!");

  display.setFont();
  display.setCursor(30, 30);
  display.print("To:");
  display.println(to.substring(0, 22));
  display.setCursor(30+14+21-5, 30+12);
  display.println(to.substring(22));

  display.setCursor(30, 30+24);
  String value = vector2string_DEC(trx_test.value,2);
  display.print("Value:");display.print(value);display.print(" ETH");
  display.setCursor(30, 30+24+12);
  String gasPrice = vector2string_DEC(trx_test.gasPrice,1);
  display.print("GasPrice:");display.print(gasPrice);display.print(" GWei");
  display.setCursor(30, 30+24+12+12);
  String gasLimit = vector2string_DEC(trx_test.gasLimit,0);
  display.print("GasLimit:");display.print(gasLimit);
  display.setCursor(30, 30+24+12+12+12);
  String nonce = vector2string_DEC(trx_test.nonce,0);
  display.print("Nonce:");display.print(nonce);
  

  display.setTextSize(1);
  display.setCursor(25, 122-15);
  display.print("ENTER to continue or DEL to cancel");
  
  display.update();
  //display.updateWindow(0, 0, 250, 122, false);
  //display.powerDown();

  while(1){
    int button_pressed = readButton();

    switch(button_pressed){
      case 0x0E:
        return 1;
        break;
      case 0x0D:
       {
        return 0;
       }
       break;  
    }
  }

}

void success_animate(){
  display.fillScreen(GxEPD_WHITE);
  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

  display.drawBitmap(lock, 0, 0, 28, 41, GxEPD_BLACK);
  display.updateWindow(0, 0,249, 122, true);
  display.drawBitmap(unlock_int, 0, 0, 28, 41, GxEPD_BLACK);
  display.updateWindow(0, 0,249, 122, true);
  display.drawBitmap(unlock, 0, 0, 28, 41, GxEPD_BLACK);
  display.updateWindow(0, 0,249, 122, true);

  /*
  uint16_t x0 = 85;
  uint16_t y0 = 70;
  int dx = 5;
  int dy = 5;
  uint16_t r = 10;
  for (int i = 0; i < 6; ++i)
  {
    Serial.print(i);Serial.print(",");Serial.print(x0);Serial.print(",");Serial.println(y0);
    
    display.fillCircle(x0,y0,r,GxEPD_BLACK);
    x0 += dx;
    y0 += dy;
    if (i%4==0)
    {
      digitalWrite(LED,HIGH);
      display.updateWindow(0, 0,249, 122, true);
      digitalWrite(LED,LOW);
    }

  }

  x0 = 115;
  y0 = 100;
  dx = 5;
  dy = -5;
  for (int i = 0; i < 15; ++i)
  {
    Serial.print(i);Serial.print(",");Serial.print(x0);Serial.print(",");Serial.println(y0);

    display.fillCircle(x0,y0,r,GxEPD_BLACK);
    x0 += dx;
    y0 += dy;
    if (i%4==0)
    {
      digitalWrite(LED,HIGH);
      display.updateWindow(0, 0,249, 122, true);
      digitalWrite(LED,LOW);
    }
  }
*/
}

void display_token_drawing(){
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(Hitcon_01, 0, 0, 250, 122, GxEPD_BLACK);
  display_static();
  display.update();
  //display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  display.powerDown();
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
    //display_static();
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