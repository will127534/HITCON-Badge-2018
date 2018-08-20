//NFC
#include "PN532_I2C.h"
#include "emulatetag.h"
#include "NdefMessage.h"


//BLE
#include "epddisplay.hpp"
#include <Fonts/Roboto_Medium12pt7b.h>
#include <Fonts/Roboto_Medium8pt7b.h>
#include "icon.h"
#include "BLE.hpp"
//Read buttons, battery...
#include "control.hpp"
#include <hal_adc.h>

#include "cmdline_interface.hpp"
#include <Cmd.h>
//Print and converting String etc...
#include "util.hpp"
//transaction related
#include "wallet.hpp"
//saving the data
#include "LFlash.h"
//NFC
#define PN532_IRQ   (2)
#define PN532_RESET (17)  

#include <LWiFi.h>

PN532_I2C pn532i2c(Wire);
EmulateTag nfc(pn532i2c);

void init_hardware(){
  Serial.begin(115200);
  Serial.println("[Setup] Program Start");
  delay(100);

  //Pin Setup
  pinMode(VBAT_PIN,INPUT);
  pinMode(ABUTTON_PIN,INPUT);
  pinMode(PN532_RESET,OUTPUT);
  digitalWrite(PN532_RESET,LOW);
  pinMode(LED,OUTPUT);
  hardware_wallet.init(SE_SS);

  //MT7697's function Setup 
  if (LFlash.begin() != LFLASH_OK)
  {
      Serial.println("Flash init failed.");
      return;
  }
  randomNumbergenerator_init();
  LBLE.begin();
  //MT7697 will start Wifi after BLE init, so we need to close it to save power.
  WiFi.end();


}

void setup(void)
{
  init_hardware();
  Serial.println("[Setup] Hardware setup done");
  if (readButton()==0x0B) // cmdline interface
  {
    init_cmdline_interface();
    while(1){
      cmdPoll();
    }
  }
  init_display();
  Serial.println("[Setup] Display setup done");
  //Bootstarping 
  if (readButton()==0x0A) // Welcome Page
  {
    display.fillScreen(GxEPD_WHITE);
    display.drawBitmap(HITCON, 30, 0, 193, 121, GxEPD_BLACK);
    display.update();
    while(1);
  }
  init_se();
  Serial.println("[Setup] Security Element setup done");
  init_BLE();
  Serial.println("[Setup] BLE setup done");
  main_menu();
  Serial.println("[Setup] Main menu done");
  init_cmdline_interface();
}

//counter for update BT icon
bool BT_icon_updated=0;
bool BT_icon_updated2=0;

//counter for update Battery Voltage 
uint32_t start = 0;
uint32_t lastupdate_vbat = 0;

//counter for Partial Update updating the icon/balance/battery voltage
uint8_t battery_pattial,balance_partial,BTicon_partial = 0;

uint8_t page = 1;
bool changed = 0;

uint32_t token_page_frame_counter = 0;
bool animation = 0;

void loop()
{
  cmdPoll();
  int button_pressed = readButton();

  switch(button_pressed){
    case 0x04:
      {
        if(page <= 1){
          page = total_page;
          changed = 1;
        }
        else{
          page--;
          changed = 1;
        }
        Serial.print("[Main] Page--, Curent Page = ");Serial.println(page);
      }
      break;  
    case 0x06:
     {
        if(page==total_page){
          page = 1;
          changed = 1;
        }
        else{
          page++;
          changed = 1;
        }
        Serial.print("[Main] Page++, Curent Page = ");Serial.println(page);
     }
     break;  
    case 0x02:
     {
        if(page==SETTINGPAGE){
          BLE_pairing();
        }       
        break;  
     }
    case 0x05:
     {
        if(page==SETTINGPAGE){
          //clear_ERC20();
          changed = 1;
        }       
        break;  
     }
    case 0x08:
     {
        if(page==SETTINGPAGE){
          ReinitWallet();
          changed = 1;
        }       
        break;  
     }
     
  }

  if (changed)
  {
    switch(page){
      case MAINPAGE:
        main_menu();
        break;  
      case SETTINGPAGE:
        setting_menu();
       break;  
      case TOKENPAGE:
        display_token_drawing();
        token_page_frame_counter = 0;
        animation = 1;
        break;
      case VCARDPAGE:
        display_Vcard();
        break;
    }
   changed = 0;
  }


  //Partial Update for Icon/Voltage/Balance
  if(LBLEPeripheral.connected()>0 && BT_icon_updated == 0){
        BT_icon_updated = 1;
        Serial.println("[Main] BLE Connected");
        display.drawBitmap(BT, 0, 2, 20, 20, GxEPD_BLACK);
        display.updateWindow(0, 2, 20, 20, true);
        display.powerDown();
        BTicon_partial++;
  }
  if(LBLEPeripheral.connected() == 0 && BT_icon_updated == 1){
        BT_icon_updated = 0;
        Serial.println("[Main] BLE Disconnected");
        display.fillRect(0, 2, 20, 20,GxEPD_WHITE);
        display.updateWindow(0, 2, 20, 20, true);
        display.powerDown();
        BTicon_partial++;
  }
  if (millis()-lastupdate_vbat>5*60*1000){
        lastupdate_vbat = millis();
        Serial.print("[Main] Battery:");Serial.print(readBatteryPercentage());Serial.println("%");
        display.fillRect(190, 0, 59, 30,GxEPD_WHITE);
        display.setFont(&Roboto_Medium8pt7b);
        display.setCursor(190, 15);
        display.print(readBatteryVoltage(),3);
        display.println(" V");
        display.updateWindow(190, 0, 59, 15, true);
        display.powerDown();
        battery_pattial++;

        uint8_t battery_percentage = readBatteryPercentage();
        uint32_t size_battery = 1;
        Battery_Level_GATT->setValueBuffer(&battery_percentage,size_battery);
  }
  if (BTicon_partial>10 || balance_partial>5 || battery_pattial>5 || token_page_frame_counter>8){
        changed = 1;
        battery_pattial=0;
        balance_partial=0;
        BTicon_partial=0;
        token_page_frame_counter = 0;
  }

  if (page == MAINPAGE)
  {
      if (NewBalanceFlag)
      {
        NewBalanceFlag = 0;
        //partial update balance
        display.fillRect(0, 50, 150, 80, GxEPD_WHITE);
        display.setCursor(30, 82);

        double Balance,HITCON_Balance;
        uint32_t size = 8;
        LFlash.read("Wallet","Balance",(uint8_t *)&Balance,&size);
        LFlash.read("Wallet","HitconBalance",(uint8_t *)&HITCON_Balance,&size);
        Serial.print("[Main] New Balance:");Serial.println(Balance);
        Serial.print("[Main] New HITCON_Balance:");Serial.println(HITCON_Balance);

        display.setFont(&Roboto_Medium8pt7b);
        String balance_str = String(Balance,4) + " ETH";
        String HITCON_Balance_str =  String(int(HITCON_Balance)) + " Hitcon";
        int16_t x1,y1;
        uint16_t w,h;

        int16_t x2,y2;
        uint16_t w2,h2;
  
        display.getTextBounds((char*)balance_str.c_str(), 0, 0, &x1, &y1, &w, &h);
        display.setCursor((150-w)/2,73);
        display.print(balance_str);

        display.getTextBounds((char*)HITCON_Balance_str.c_str(), 0, 0, &x2, &y2, &w2, &h2);
        display.setCursor((150-w2)/2,93);
        display.print(HITCON_Balance_str);

        display.updateWindow(0, 50, 150, 80, true);
        display.powerDown();
        balance_partial++;
      }
  }

  if (page == TOKENPAGE)
  {
      if (NewBalanceFlag){
        animation = 1;
      }
      NewBalanceFlag = 0;
  }

  if (page == TOKENPAGE && animation == 1)
  {
    double HITCON_Balance;
    uint32_t size = 8;
    LFlash.read("Wallet","HitconDisplayBalance",(uint8_t *)&HITCON_Balance,&size);
    Serial.print("[Main] HitconDisplayBalance:");
    Serial.println(HITCON_Balance);
    if(HITCON_Balance<10){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L0, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
        animation=0;
    }
    if(HITCON_Balance>=10 && HITCON_Balance<20){
      if (token_page_frame_counter%2 == 0){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L1, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
      else{
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L1a, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
    }
    if(HITCON_Balance>=20 && HITCON_Balance<40){
      if (token_page_frame_counter%2 == 0){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L2, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
      else{
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L2a, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
    }
    if(HITCON_Balance>=40 && HITCON_Balance<80){
      if (token_page_frame_counter%2 == 0){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L3, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
        animation=0;
      }
    }
    if(HITCON_Balance>=80 && HITCON_Balance<200){
      if (token_page_frame_counter%2 == 0){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L3a, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
      else{
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L4, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
    }
    if(HITCON_Balance>=200 && HITCON_Balance<290){
      if (token_page_frame_counter%2 == 0){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L4a, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
      else{
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L5, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
      }
    }
    if(HITCON_Balance>=290 && HITCON_Balance<350){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L5a, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
        animation=0;
    }
    if(HITCON_Balance>=350){
        display.fillScreen(GxEPD_WHITE);
        display.drawBitmap(Hitcon_L7, 0, 0, 250, 122, GxEPD_BLACK);
        display_static();
        display.updateWindow(0, 0, 249, 122, true);
        animation=0;
    }
    token_page_frame_counter++;
  }
  if (Process_BLE())
  {
    changed = 1;
  }
  delay(80);
}
