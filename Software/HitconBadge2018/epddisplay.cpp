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
#include "english_mnemonic.h"
#include <nvdm.h>
#include "cmdline_interface.hpp"
#include <Cmd.h>
GxIO_Class io(SPI, 10, 7, 5);
GxGDE0213B1 display(io,5,4);
extern uint8_t page;


void init_display(){

  display.init();
  display.setTextColor(GxEPD_BLACK);
  display.setRotation(3);
  display.fillScreen(GxEPD_WHITE);
}


//Battery_voltage ... etc
void display_static(){

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  if(LBLEPeripheral.connected()>0){
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

  if (page != 1)
  {
    display.fillTriangle(x0,y0,x0,y1,5,61, GxEPD_BLACK);
    display.setCursor(4, 51);
    display.println("4");
  }
  if (page !=total_page)
  {
    display.fillTriangle(x1,y0,x1,y1,245,61, GxEPD_BLACK);
    display.setCursor(246-8, 51);
    display.println("6");
  }

  //display.drawBitmap(NFC, 50, 0, 20, 20, GxEPD_BLACK);
}


void main_menu(){
  display.fillScreen(GxEPD_WHITE);

  int16_t x1,y1;
  uint16_t w,h;

  display.setFont(&Roboto_Medium12pt7b);
  display.getTextBounds("Balance:", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((150-w)/2,43);
  
  display.println("Balance:");

  double Balance,HITCON_Balance,HITCON_Display_Balance;
  uint32_t size = 8;

  LFlashStatus status = LFlash.read("Wallet","Balance",(uint8_t *)&Balance,&size);
  if (status == LFLASH_ITEM_NOT_FOUND)
  {
    Balance = 0;
    LFlash.write("Wallet","Balance",LFLASH_RAW_DATA,(const uint8_t *)&Balance,sizeof(Balance));
  }
  
  status = LFlash.read("Wallet","HitconBalance",(uint8_t *)&HITCON_Balance,&size);
  if (status == LFLASH_ITEM_NOT_FOUND)
  {
    HITCON_Balance = 0;
    LFlash.write("Wallet","HitconBalance",LFLASH_RAW_DATA,(const uint8_t *)&HITCON_Balance,sizeof(HITCON_Balance));
  }

  status = LFlash.read("Wallet","HitconDisplayBalance",(uint8_t *)&HITCON_Display_Balance,&size);
  if (status == LFLASH_ITEM_NOT_FOUND)
  {
    HITCON_Display_Balance = 0;
    LFlash.write("Wallet","HitconDisplayBalance",LFLASH_RAW_DATA,(const uint8_t *)&HITCON_Display_Balance,sizeof(HITCON_Display_Balance));
  }


  display.setFont(&Roboto_Medium8pt7b);
        
        Serial.print("[Main] Balance:");Serial.println(Balance);
        Serial.print("[Main] HITCON_Balance:");Serial.println(HITCON_Balance);
        
        String balance_str = String(Balance,4) + " ETH";
        String HITCON_Balance_str = String(int(HITCON_Balance)) + " Hitcon";
        int16_t x2,y2;
        uint16_t w2,h2;
  
        display.getTextBounds((char*)balance_str.c_str(), 0, 0, &x1, &y1, &w, &h);
        display.setCursor((150-w)/2,73);
        display.print(balance_str);

        display.getTextBounds((char*)HITCON_Balance_str.c_str(), 0, 0, &x2, &y2, &w2, &h2);
        display.setCursor((150-w2)/2,93);
        display.print(HITCON_Balance_str);

  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(4)];
  String QR_code_string = String("0x")+publicAddress_string;
  Serial.print("[Main Menu] QR code data:");Serial.println(QR_code_string);
  qrcode_initText(&qrcode, qrcodeData, 4, 0, QR_code_string.c_str());

  display.setCursor(150,100);
  //display.println("Address: ");

  int x0 = 150;
  int y0 = 30;
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
  uint16_t text_x0 = 30;
  uint16_t text_y0 = 44;
  
  display.setCursor(text_x0, text_y0);
  display.print("2   BLE Re-Paring");

  display.setCursor(text_x0, text_y0+isolation*1);
  display.print("8   Reinit Wallet");

  display.setCursor(text_x0, text_y0+isolation*2);
  //display.print("8   Reinit Wallet");
  
  display_static();
  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  display.powerDown();
}
void display_Vcard(){

  char Vcard[250] = {0};
  char Vcard_name[100] = {0};
      
  bool Vcard_get = 0;
  nvdm_status_t status;

  uint32_t size = 250;
  uint32_t size_name = 100;

  status = nvdm_read_data_item("VCARD", "VCARD", (uint8_t*)Vcard, &size);

  if (status != NVDM_STATUS_OK) {
      Serial.println("[display_Vcard] No Vcard found");
  }
  else{
      Serial.println("[display_Vcard] Vcard found");
      nvdm_read_data_item("VCARD", "Name", (uint8_t*)Vcard_name, &size_name);
      Serial.print("[display_Vcard] Vcard Size:");Serial.println(size);
      Vcard_get = 1;
  }

  display.fillScreen(GxEPD_WHITE);
  display.setFont(&Roboto_Medium8pt7b);
  if (Vcard_get)
  {
      Serial.println("[display_Vcard] QR code:");Serial.println(Vcard);
      Serial.print("[display_Vcard] QR code length:");Serial.println(qrcode_getBufferSize(9));

      QRCode qrcode;
      uint8_t qrcodeData[qrcode_getBufferSize(9)];
      qrcode_initBytes(&qrcode, qrcodeData, 9, ECC_LOW,(uint8_t*)Vcard,size);

      int x0 = 25;
      int y0 = 8;
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

      
      display.setFont(&Roboto_Medium8pt7b);
      int16_t x1,y1;
      uint16_t w,h;
      display.getTextBounds(Vcard_name, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((250-131-w)/2+131,(122-h)/2+h);
      display.println(Vcard_name);
  }
  else{
      display.setFont(&Roboto_Medium8pt7b);
      int16_t x1,y1;
      uint16_t w,h;
      display.getTextBounds("Vcard Not found", 0, 0, &x1, &y1, &w, &h);
      display.setCursor(20,50);
      display.println("Vcard Not found");
      display.setCursor(20,80);
      display.println("Please Import from Terminal");
  }

  
  display_static();
  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  display.powerDown();
}
void pincode_success(){
      display.drawBitmap(unlock_int, 200, 45, 28, 41, GxEPD_BLACK);
      display.updateWindow(200, 45,28, 41, true);
      display.drawBitmap(unlock, 200, 45, 28, 41, GxEPD_BLACK);
      display.updateWindow(200, 45,28, 41, true);
}
void pincode_error(){

  int16_t x1,y1;
  uint16_t w,h;

  display.fillScreen(GxEPD_WHITE);
  display.setFont(&Roboto_Medium8pt7b);
  
  display.getTextBounds("Pin Code Error!", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((250-w)/2 , 40);
  display.print("Pin Code Error!");
  display.getTextBounds("Canceled Transaction", 0, 0, &x1, &y1, &w, &h);
  display.setCursor((250-w)/2 , 60);
  display.print("Canceled Transaction");
  display.update();
}
bool setpincode_page(uint8_t *pincode){

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  display.fillScreen(GxEPD_WHITE);

  display.setFont(&Roboto_Medium8pt7b);
  uint16_t x0 = 20;

  display.setCursor(x0, 50);
  display.print("Please Set a 6 digit Pin Code:");

  display.updateWindow(200, 45,28, 41, true);
  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

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
      for (int i = 0; i < 6; ++i)
      {
        pincode[i] += 0x30;
      }
      display.setTextSize(0);
      return 1;
    }
    if (button_pressed<0x0A && pointer<6 && button_pressed != -1)
    {
      pincode[pointer] = button_pressed;
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
bool pincode_page(uint8_t *pincode,uint8_t retry){

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  display.fillScreen(GxEPD_WHITE);

  display.setFont(&Roboto_Medium8pt7b);
  uint16_t x0 = 20;

  if (retry > 0)
  {
    display.setCursor(x0, 25);
    display.print("Please Enter Pin Code: ");
    display.setCursor(x0, 50);
    display.print("Retry:");display.print(retry);display.print("/3");
  }
  else{
    display.setCursor(x0, 50);
    display.print("Please Enter Pin Code:");
  }
  
  display.drawBitmap(lock, 200, 45, 28, 41, GxEPD_BLACK);
  display.updateWindow(200, 45,28, 41, true);
  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

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
      display.setTextSize(0);
      for (int i = 0; i < 6; ++i)
      {
        pincode[i] += 0x30;
      }
      return 1;
    }
    if (button_pressed<0x0A && pointer<6 && button_pressed != -1)
    {
      pincode[pointer] = button_pressed;
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
bool confirm_pincode_page(uint8_t *pincode,uint8_t retry){

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  display.fillScreen(GxEPD_WHITE);

  display.setFont(&Roboto_Medium8pt7b);
  uint16_t x0 = 20;

  if (retry > 0)
  {
    display.setCursor(x0, 25);
    display.print("Please Confirm Pin Code:");
    display.setCursor(x0, 50);
    display.print("Retry:");display.print(retry);display.print("/3");
  }
  else{
    display.setCursor(x0, 50);
    display.print("Please Confirm Pin Code:");
  }
  
  display.drawBitmap(lock, 200, 45, 28, 41, GxEPD_BLACK);
  display.updateWindow(200, 45,28, 41, true);
  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

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
      display.setTextSize(0);
      for (int i = 0; i < 6; ++i)
      {
        pincode[i] += 0x30;
      }
      return 1;
    }
    if (button_pressed<0x0A && pointer<6 && button_pressed != -1)
    {
      pincode[pointer] = button_pressed;
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
bool Token_transaction_page(transaction_struct trx_test,String token_name){

  display.setFont(&Roboto_Medium8pt7b);
  display.setTextSize(0);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");

  std::vector<uint8_t> token_to(trx_test.data.begin() + 16, trx_test.data.begin() + 36);
  std::vector<uint8_t> token_value(trx_test.data.begin() + 36, trx_test.data.end());

  String to = "0x" + vector2string(token_to);

  display.fillScreen(GxEPD_WHITE);
  display.updateWindow(0, 0, 250, 122, false);

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(10, 20);
  display.println("New Tranaction!");

  display.setFont();
  display.setCursor(10, 30);
  display.print("To:");
  display.println(to.substring(0, 22));
  display.setCursor(10+14+21-5, 30+12);
  display.println(to.substring(22));

  String value = vector2string_DEC(token_value,2);
  String value_string = "Value:" + value + " " + String(token_name);
  display.setFont();
  display.setCursor(10, 30+24);
  display.print(value_string);


  display.setCursor(10, 30+24+12);
  String gasPrice = vector2string_DEC(trx_test.gasPrice,1);
  display.print("GasPrice:");display.print(gasPrice);display.print(" GWei");
  display.setCursor(10, 30+24+12+12);
  String gasLimit = vector2string_DEC(trx_test.gasLimit,0);
  display.print("GasLimit:");display.print(gasLimit);
  display.setCursor(10, 30+24+12+12+12);
  String nonce = vector2string_DEC(trx_test.nonce,0);
  display.print("Nonce:");display.print(nonce);
  
  display.setTextSize(1);
  display.setCursor(25, 122-15);
  display.print("ENTER to continue or DEL to cancel");

  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  //display.updateWindow(0, 0, 250, 122, false);
  //display.powerDown();
  uint32_t time_start = millis();
  while(1){ 
    if (millis() - time_start > 50*1000) // 50s timeout
    {
      return 0;
    }
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
bool transaction_page(transaction_struct *trx_test){
  uint8_t page = 0;
  //detect if it is token transfer
  std::vector<String> token_names;
  token_names.push_back("ETH");
  for (int i = 0; i < erc20s.size(); ++i)
  {
    token_names.push_back(String(erc20s[i].Name));
  }

  display.setFont(&Roboto_Medium8pt7b);
  display.setTextSize(0);
  display.setCursor(190, 15);
  display.print(readBatteryVoltage(),3);
  display.println(" V");



  String to = "0x" + vector2string(trx_test->to);



  display.fillScreen(GxEPD_WHITE);
  display.updateWindow(0, 0, 250, 122, false);

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor(10, 20);
  display.println("New Tranaction!");

  display.setFont();
  display.setCursor(10, 30);
  display.print("To:");
  display.println(to.substring(0, 22));
  display.setCursor(10+14+21-5, 30+12);
  display.println(to.substring(22));

  String value = vector2string_DEC(trx_test->value,2);
  String value_string = "Value:" + value + " ETH";
  display.setFont();
  display.setCursor(10, 30+24);
  display.print(value_string);


  display.setCursor(10, 30+24+12);
  String gasPrice = vector2string_DEC(trx_test->gasPrice,1);
  display.print("GasPrice:");display.print(gasPrice);display.print(" GWei");
  display.setCursor(10, 30+24+12+12);
  String gasLimit = vector2string_DEC(trx_test->gasLimit,0);
  display.print("GasLimit:");display.print(gasLimit);
  display.setCursor(10, 30+24+12+12+12);
  String nonce = vector2string_DEC(trx_test->nonce,0);
  display.print("Nonce:");display.print(nonce);
  
  display.setTextSize(1);
  display.setCursor(25, 122-15);
  display.print("ENTER to continue or DEL to cancel");
  

  uint16_t x0 = 160;
  uint16_t x1 = 220;

  uint16_t y0 = 66;
  uint16_t y1 = 76;
  display.setFont(&Roboto_Medium8pt7b);
  display.fillTriangle(x0,y0,x0,y1,x0-5,y1-5, GxEPD_BLACK);
  display.setCursor(x0-5-12, y1);
  display.println("4");
  
  display.fillTriangle(x1,y0,x1,y1,x1+5,y1-5, GxEPD_BLACK);
  display.setCursor(x1+5+3,y1);
  display.println("6");

  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);
  //display.updateWindow(0, 0, 250, 122, false);
  //display.powerDown();
  
  uint8_t token_count = token_names.size();
  bool changed = 1;
  uint16_t box_x0 = x0+10;
  uint16_t box_x1 = x1-10;
  uint16_t box_width = box_x1 - box_x0;
  uint16_t box_y0 = 50;
  uint16_t box_height = 55;
  
  while(1){ 
    int button_pressed = readButton();

    switch(button_pressed){
      case 0x04:
        {
          if(page <= 0){
            page = token_count-1;
            changed = 1;
          }
          else{
            page--;
            changed = 1;
          }
          Serial.print("[transaction_page] pointer--, Curent pointer = ");Serial.println(page);
        }
        break;  
      case 0x06:
       {
          if(page==token_count-1){
            page = 0;
            changed = 1;
          }
          else{
            page++;
            changed = 1;
          }
          Serial.print("[transaction_page] pointer++, Curent pointer = ");Serial.println(page);
       }
       break;  
      case 0x0E:
        if (page != 0)
        {
          Serial.print("[transaction_page] User Modified to Token!");
          uint8_t transfermethod[4] = {0xa9,0x05,0x9c,0xbb};
          std::vector<uint8_t> data(transfermethod,transfermethod+4);

          for (int i = 0; i < 12; ++i)
          {
            data.push_back(0x00);
          }
          data.insert(data.end(), trx_test->to.begin(), trx_test->to.end());

          for (int i = 0; i < 32-trx_test->value.size(); ++i)
          {
            data.push_back(0x00);
          }
          data.insert(data.end(), trx_test->value.begin(), trx_test->value.end());
          trx_test->data.clear();
          trx_test->data.insert(trx_test->data.end(), data.begin(), data.end());
          trx_test->value.clear();
          trx_test->value.push_back(0x00);
          trx_test->to.clear();

          uint8_t GasLimit100000[3] = {0x01,0x86,0xA0};
          trx_test->gasLimit.clear();
          trx_test->gasLimit.assign(GasLimit100000, GasLimit100000 + 3);

          for (int i = 0; i < 20; ++i)
          {
            trx_test->to.push_back(erc20s[page-1].Contract_Address[i]);
          }
          Serial.println("\tTo:");
          print_vector(trx_test->to);Serial.println();
          Serial.println("\tData:");
          print_vector(trx_test->data);Serial.println();
          Serial.println("\tValue:");
          print_vector(trx_test->value);Serial.println();

        }
        return 1;
        break;
      case 0x0D:
       {
        return 0;
       }
       break;  
    }

    if (changed)
    {
      
      digitalWrite(LED,HIGH);
      display.fillRect(box_x0, box_y0, box_width, box_height,GxEPD_WHITE);
      
      int16_t  x1_value, y1_value;
      uint16_t w_value, h_value;
      uint8_t dec = 2;
      String value = vector2string_DEC(trx_test->value,dec);
      String value_string = "Value:" + value + " " + String(token_names[page]);
      display.setFont();
      display.getTextBounds((char*)value_string.c_str(),10,30+24, &x1_value, &y1_value, &w_value, &h_value);
      display.setCursor(10, 30+24);
      display.fillRect(x1_value, y1_value, w_value, h_value,GxEPD_WHITE);
      display.print(value_string);

      display.setFont(&Roboto_Medium8pt7b);
      int16_t  x1, y1;
      uint16_t w, h;
      display.getTextBounds((char*)token_names[page].c_str(), 0, 0, &x1, &y1, &w, &h);
      int t = (box_width - w)/2+box_x0;

      display.setCursor(t, 67);
      display.print(token_names[page].c_str());

      String location = String(page+1)+"/"+String(token_count);
      display.getTextBounds((char*)location.c_str(), 0, 0, &x1, &y1, &w, &h);
      t = (box_width - w)/2+box_x0;

      display.setCursor(t, 67+18);
      display.print(location);
      display.updateWindow(box_x0, box_y0, box_width, box_height, true);
      display.updateWindow(x1_value, y1_value, w_value, h_value, true);
      digitalWrite(LED,LOW);
      changed = 0;
    }

  }

}

void clear_ERC20(){
  display.fillScreen(GxEPD_WHITE);

  uint16_t x0 = 70;
  uint16_t x1 = 180;

  uint16_t y0 = 86;
  uint16_t y1 = 96;
  display.setFont(&Roboto_Medium8pt7b);
  display.fillTriangle(x0,y0,x0,y1,x0-5,y1-5, GxEPD_BLACK);
  display.setCursor(x0-5-12, y1);
  display.println("4");
  
  display.fillTriangle(x1,y0,x1,y1,x1+5,y1-5, GxEPD_BLACK);
  display.setCursor(x1+5+3,y1);
  display.println("6");
  
  int16_t  x1_title, y1_title;
  uint16_t w_title, h_title;
  display.getTextBounds("ERC20 DataBase:", 0, 0, &x1_title, &y1_title, &w_title, &h_title);

  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor( (250-w_title) / 2, 20);
  display.println("ERC20 DataBase:");
  display.setFont();
  display.setCursor(15, 30);
  display.print("Press DEL to delete");
  display.setCursor(15, 30+9);
  display.print("Press ENTER to continue...");
  display.setFont(&Roboto_Medium8pt7b);

  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

  //display Tokens
  int page = 0;
  uint8_t token_size = erc20s.size();
  bool changed = 1;
  uint16_t box_x0 = x0+10;
  uint16_t box_x1 = x1-10;
  uint16_t box_width = box_x1 - box_x0;
  uint16_t box_y0 = 60;
  uint16_t box_height = 60;

  while(1){
    int button_pressed = readButton();
    switch(button_pressed){
      case 0x04:
        {
          if(page <= 0){
            page = token_size-1;
            changed = 1;
          }
          else{
            page--;
            changed = 1;
          }
          Serial.print("[clear_ERC20] Page--, Curent Page = ");Serial.println(page);
        }
        break;  
      case 0x06:
       {
          if(page==token_size-1){
            page = 0;
            changed = 1;
          }
          else{
            page++;
            changed = 1;
          }
          Serial.print("[clear_ERC20] Page++, Curent Page = ");Serial.println(page);
       }
       break;  
      case 0x0E:
       {
          return;
          break;  
       }
      case 0x0D:
       {
          Serial.print("[clear_ERC20] Deleting ERC20");Serial.print(erc20s[page].Name);
          nvdm_delete_data_item("ERC20", erc20s[page].Name);
          erc20s.erase(erc20s.begin()+page);
          page = 0;
          token_size = erc20s.size();
          if (erc20s.size()==0)
          {
            page = -1;
          }
          changed = 1;
          break;  
       }
    }
    if (changed)
    {
      display.setFont(&Roboto_Medium8pt7b);
      digitalWrite(LED,HIGH);
      display.fillRect(box_x0, box_y0, box_width, box_height,GxEPD_WHITE);
      int16_t  x1, y1;
      uint16_t w, h;
      int t;
      if (page>=0)
      {
        display.getTextBounds((char*)erc20s[page].Name, 0, 0, &x1, &y1, &w, &h);
        t = (box_width - w)/2+box_x0;
        display.setCursor(t, 85);
        display.print((char*)erc20s[page].Name);
      }
      String location = String(page+1)+"/"+String(token_size);
      display.getTextBounds((char*)location.c_str(), 0, 0, &x1, &y1, &w, &h);
      t = (box_width - w)/2+box_x0;

      display.setCursor(t, 85+18);
      display.print(location);
      display.updateWindow(box_x0, box_y0, box_width, box_height, true);
      digitalWrite(LED,LOW);
      changed = 0;
    }
  }
  return;

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
void ReinitWallet(){

  display.fillScreen(GxEPD_WHITE);
  display.setCursor(10,30);
  display.setFont(&Roboto_Medium8pt7b);
  display.print("Init Wallet:");
  display.setFont();
  display.setCursor(10,50);
  display.print("Press 1 to input Memonic code");
  display.setCursor(10,60);
  display.print("from console");
  display.setCursor(10,75);
  display.print("Press ENTER to generate random key");
  display.setCursor(10,90);
  display.print("Press DEL to return to menu");
  display.update();
  bool console_input = 0;
  while(1){
    int button_pressed = readButton();
    if(button_pressed == 0x01){
      console_input = 1;
      break;
    }
    if(button_pressed == 0x0E){
      console_input = 0;
      break;
    }
    if(button_pressed == 0x0D){
      return;
    }
  }
  LBLEPeripheral.disconnectAll();
  LBLEPeripheral.stopAdvertise();
  String Memonic;
  if (console_input)
  {
    Memonic = console_input_mnemonic();
  }
  else{
    Memonic = randomgeneratewallet();
  }
  Serial.print("[ReinitWallet] ");Serial.println(Memonic);
  reinit_secureelement(Memonic);
  BLE_pairing();

}

String randomgeneratewallet(){
  //generate ENT here
  uint8_t entropy[ENT_BITS/8] = {0};
  for (int i = 0; i < 8; ++i)
  {
    uint32_t a_random_number =  randomUint32_t_generator();
    memcpy((void*)entropy+i*4,(const void*)&a_random_number,4);
  }
  Serial.print("[randomgeneratewallet] entropy:");
  for (int i = 0; i < ENT_BITS/8; ++i)
  {
    Serial.print(entropy[i],HEX);Serial.print(" ");
  }
  Serial.println();

  uint8_t sha256ed[32] = {0};
  sha256_data( entropy, ENT_BITS/8, sha256ed);

  Serial.print("[randomgeneratewallet] sha256 hashed:");
  for (int i = 0; i < 32; ++i)
  {
    Serial.print(sha256ed[i],HEX);Serial.print(" ");
  }
  Serial.println();
  
  //by BIP39, taking the first ENT/32 bit as checksum and append to ENT
  uint8_t mnemonic_with_checksum[ENT_BITS/8+1] = {0};
  memcpy(mnemonic_with_checksum,entropy,ENT_BITS/8);
  memcpy(mnemonic_with_checksum+ENT_BITS/8,sha256ed,1);

  Serial.print("[randomgeneratewallet] mnemonic_with_checksum:");
  for (int i = 0; i < ENT_BITS/8+1; ++i)
  {
    Serial.print(mnemonic_with_checksum[i],HEX);Serial.print(" ");
  }
  Serial.println();

  String mnemonic_string = ArraytoString_binary(mnemonic_with_checksum,ENT_BITS/8+1);
  Serial.println(mnemonic_string);
  
  uint16_t mnemonic_byte[WORD_COUNT] = {0};
  for (int i = 0; i < WORD_COUNT; ++i)
  {
    mnemonic_byte[i] = BinaryStringtoArray(mnemonic_string.substring(i*11,(i+1)*11));
  }

  Serial.print("[randomgeneratewallet] mnemonic_byte:");
  for (int i = 0; i < WORD_COUNT; ++i)
  {
    Serial.print(mnemonic_byte[i],HEX);Serial.print(" ");
  }
  Serial.println();

  String Memonic_sentance;
  String Memonic_words[WORD_COUNT]; 
  uint8_t pointer = 0;
  for (int i = 0; i < WORD_COUNT; ++i)
  {
    Memonic_words[i] = en_i[mnemonic_byte[i]%2048];
    //Serial.println(Memonic_words[i]);
    Memonic_sentance += Memonic_words[i];
    if (i < WORD_COUNT-1)
    {
      Memonic_sentance += " ";
    }
  }

  Serial.print("[randomgeneratewallet] mnemonic sentance:");Serial.println(Memonic_sentance);

  display.fillScreen(GxEPD_WHITE);

  uint16_t x0 = 70;
  uint16_t x1 = 180;

  uint16_t y0 = 86;
  uint16_t y1 = 96;
  display.setFont(&Roboto_Medium8pt7b);
  display.fillTriangle(x0,y0,x0,y1,x0-5,y1-5, GxEPD_BLACK);
  display.setCursor(x0-5-12, y1);
  display.println("4");
  
  display.fillTriangle(x1,y0,x1,y1,x1+5,y1-5, GxEPD_BLACK);
  display.setCursor(x1+5+3,y1);
  display.println("6");
  
  int16_t  x1_title, y1_title;
  uint16_t w_title, h_title;
  display.getTextBounds("Mnemonic Code:", 0, 0, &x1_title, &y1_title, &w_title, &h_title);


  display.setFont(&Roboto_Medium8pt7b);
  display.setCursor( (250-w_title) / 2, 20);
  display.println("Mnemonic Code:");
  display.setFont();
  display.setCursor(15, 30);
  display.print("Please write down the Mnemonic Code");
  display.setCursor(15, 30+9);
  display.print("to a safe location");
  display.setCursor(15, 30+18);
  display.print("Press ENTER to continue...");
  display.setFont(&Roboto_Medium8pt7b);

  display.update();
  display.updateWindow(0, 0, GxEPD_WIDTH, GxEPD_HEIGHT, false);

  //display Memonic code
  uint8_t page = 0;
  bool changed = 1;
  uint16_t box_x0 = x0+10;
  uint16_t box_x1 = x1-10;
  uint16_t box_width = box_x1 - box_x0;
  uint16_t box_y0 = 60;
  uint16_t box_height = 60;

  while(1){
    int button_pressed = readButton();
    switch(button_pressed){
      case 0x04:
        {
          if(page <= 0){
            page = WORD_COUNT-1;
            changed = 1;
          }
          else{
            page--;
            changed = 1;
          }
          Serial.print("[randomgeneratewallet] Page--, Curent Page = ");Serial.println(page);
        }
        break;  
      case 0x06:
       {
          if(page==WORD_COUNT-1){
            page = 0;
            changed = 1;
          }
          else{
            page++;
            changed = 1;
          }
          Serial.print("[randomgeneratewallet] Page++, Curent Page = ");Serial.println(page);
       }
       break;  
      case 0x0E:
       {
          return Memonic_sentance;
          break;  
       }
    }
    if (changed)
    {
      display.setFont(&Roboto_Medium8pt7b);
      digitalWrite(LED,HIGH);
      display.fillRect(box_x0, box_y0, box_width, box_height,GxEPD_WHITE);
      
      int16_t  x1, y1;
      uint16_t w, h;
      display.getTextBounds((char*)Memonic_words[page].c_str(), 0, 0, &x1, &y1, &w, &h);
      int t = (box_width - w)/2+box_x0;

      display.setCursor(t, 85);
      display.print(Memonic_words[page]);

      String location = String(page+1)+"/"+String(WORD_COUNT);
      display.getTextBounds((char*)location.c_str(), 0, 0, &x1, &y1, &w, &h);
      t = (box_width - w)/2+box_x0;

      display.setCursor(t, 85+18);
      display.print(location);
      display.updateWindow(box_x0, box_y0, box_width, box_height, true);
      digitalWrite(LED,LOW);
      changed = 0;
    }
  }
  return Memonic_sentance;
}

String console_input_mnemonic(){
  //Serial.println(WORD_COUNT);
  while(Serial.available()){
      char c = Serial.read();
  }
  Serial.println("Please input mnemonic at a time:");
  String Memonic_string;

  while(!Serial.available()){
    delay(100);
  }
  while(Serial.available()){
    char c = Serial.read();
    if (c == '\n' || c == '\r')
    { 
      break;
    }
    if (c>=0x41 && c<=0x7A || c == 0x20)
    {
      Memonic_string += c;
    }
  }
  while(Memonic_string.endsWith(" ")){
    Memonic_string.remove(Memonic_string.length()-1);
  }
  
  Serial.print("[console_input_mnemonic] Get:");Serial.println(Memonic_string);
  return Memonic_string;
}
//The Real display will be on main code, just init the display for it
void display_token_drawing(){
    display.fillScreen(GxEPD_WHITE);
    display_static();
    display.update();
}
void BLE_pairing(){
  bool changed = 1;
  LFlash.write("BLE","NewSetting",LFLASH_RAW_DATA,(const uint8_t *)&changed,sizeof(changed));
  display.fillScreen(GxEPD_WHITE);
  display.setFont();
  display.setCursor(10, 30);
  display.print("Please push RST button to reset");
  display.update();
  while(1){
    delay(1000);
  }
  //WDT_Reset();
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
    Serial.print("[LBLE Pairing] AES Key:");Serial.println(AES_buf);


    sprintf (AES_key_buffer, "hitcon://pair?v=18&a=%s&k=%0s&s=%s&c=%s%s%s%s%s%s",publicAddress_string.c_str(), AES_buf,ServiceUUID,UUIDHead[0],UUIDHead[1],UUIDHead[2],UUIDHead[3],UUIDHead[4],UUIDHead[5]);

    Serial.print("[LBLE Pairing] QR code:");Serial.println(AES_key_buffer);
    
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
    LBLEPeripheral.disconnectAll();
    display.update();
    Serial.print("[LBLE Pairing] Waiting for connection");

    while(1){
      if(LBLEPeripheral.connected()>0){
        bool changed = 0;
        LFlash.write("BLE","NewSetting",LFLASH_RAW_DATA,(const uint8_t *)&changed,sizeof(changed));
        Serial.println("");
        break;
      }
      Serial.print(".");
      delay(1000);
    }
}

void secureelement_error(){
  display.fillScreen(GxEPD_WHITE);
  display.setFont();
  display.setCursor(10, 30);
  display.print("Secure Element communication error");
  display.setCursor(10, 50);
  display.print("please do a power reset");
  display.setCursor(10, 70);
  display.print("If the power reset doesn't work,");
  display.setCursor(10, 90);
  display.print("use Cmdline Interface");
  display.update();
  Serial.println("Secure Element Error -> Forced enter Cmdline");
  init_cmdline_interface();
  while(1){
    cmdPoll();
  }
}

void display_initwalletFrommemonic(){
  display.fillScreen(GxEPD_WHITE);
  display.setFont();
  display.setCursor(10, 30);
  display.print("Secure Element initing");
  display.setCursor(10, 45);
  display.print("it will take about 1 min");
  display.update();
}
