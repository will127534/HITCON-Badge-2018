//NFC
#include <Adafruit_PN532.h>
//BLE
#include "epddisplay.hpp"
#include <Fonts/Roboto_Medium12pt7b.h>
#include <Fonts/Roboto_Medium8pt7b.h>
#include "icon.h"
#include "BLE.hpp"
//Read buttons, battery...
#include "control.hpp"
#include <hal_adc.h>
#define total_page 3
#define MAINPAGE 1
#define SETTINGPAGE 2
#define TOKENPAGE 3

//Print and converting String etc...
#include "util.hpp"
//transaction related
#include "wallet.hpp"
//saving the data
#include "LFlash.h"
//NFC
#define PN532_IRQ   (2)
#define PN532_RESET (17)  


#include <hal_sleep_manager.h>

#include <LWiFi.h>
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void init_hardware(){
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);
  //pinMode(3,OUTPUT);
  //digitalWrite(3,HIGH);
  pinMode(A2,INPUT);
  pinMode(PN532_RESET,OUTPUT);
  digitalWrite(PN532_RESET,LOW);
  pinMode(LED,OUTPUT);
  if (LFlash.begin() != LFLASH_OK)
  {
      Serial.println("Flash init failed.");
      return;
  }
  randomNumbergenerator_init();
  LBLE.begin();
  WiFi.end();

}

void setup(void)
{
  init_hardware();
  Serial.println("setup done");
  init_se();
  init_display();
  Serial.println("init_display done");
  init_BLE();
  Serial.println("init_BLE done");
  
  
  main_menu();
  //delay(500);

}

bool BT_icon_updated=0;
bool BT_icon_updated2=0;
uint32_t start = 0;
uint32_t lastupdate_vbat = 0;
uint8_t battery_pattial,balance_partial,BTicon_partial = 0;



uint8_t page = 1;
bool changed = 0;
void loop()
{
/*
  for(int i =0;i<30;i++){
    Serial.print(analogRead(A1));Serial.print(" ");
  }
  Serial.println();
  delay(80);

*/

  int button_pressed = readButton();

  switch(button_pressed){
    case 0x04:
      {
        if(page <= 1){
          page = 1;
        }
        else{
          page--;
          changed = 1;
        }
        Serial.print("Page--, Curent Page = ");Serial.println(page);
      }
      break;  
    case 0x06:
     {
        if(page==total_page){
          page == 1;
        }
        else{
          page++;
          changed = 1;
        }
        Serial.print("Page++, Curent Page = ");Serial.println(page);
     }
     break;  
     case 0x0C:
     {
     }
    case 0x02:
     {
        if(page==SETTINGPAGE){
          BLE_pairing();
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
        break;
    }
   changed = 0;
  }



  if (page == 1)
  {
      if(LBLEPeripheral.connected()>0 && BT_icon_updated == 0){
        BT_icon_updated = 1;
        Serial.println("Connected");
        display.drawBitmap(BT, 0, 2, 20, 20, GxEPD_BLACK);
        display.updateWindow(0, 2, 20, 20, true);
        display.powerDown();
        BTicon_partial++;
      }
      if(LBLEPeripheral.connected() == 0 && BT_icon_updated == 1){
        BT_icon_updated = 0;
        display.fillRect(0, 2, 20, 20,GxEPD_WHITE);
        display.updateWindow(0, 2, 20, 20, true);
        display.powerDown();
        BTicon_partial++;
      }
      if (NewBalanceFlag)
      {
        NewBalanceFlag = 0;
        //partial update balance
        display.fillRect(30, 64, 100, 24,GxEPD_WHITE);
        display.setCursor(30, 80);

        double Balance;
        uint32_t size = 8;
        LFlash.read("Wallet","Balance",(uint8_t *)&Balance,&size);
        Serial.print("Balance:");Serial.println(Balance);

        display.setFont(&Roboto_Medium8pt7b);
        display.print(Balance,4);
        display.println(" ETH");

        display.updateWindow(30, 64, 100, 24, true);
        display.powerDown();
        balance_partial++;
      }
      if (millis()-lastupdate_vbat>5*60*1000)
      {
        lastupdate_vbat = millis();
        Serial.println(readBatteryPercentage());
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
      if (BTicon_partial>10 || balance_partial>5 || battery_pattial>5)
      {
        changed = 1;
        battery_pattial=0;
        balance_partial=0;
        BTicon_partial=0;

      }
  }


  
  if (Process_BLE())
  {
    changed = 1;
  }
/*
  hal_sleep_manager_set_sleep_time(80); 
  if(hal_sleep_manager_is_sleep_locked()){
          Serial.println("Cannot enter the sleep mode, as the sleep is locked");
      }
  else
      {
          if (hal_sleep_manager_set_sleep_time(80) == HAL_SLEEP_MANAGER_OK)
          {
              hal_sleep_manager_enter_sleep_mode(  HAL_SLEEP_MODE_LEGACY_SLEEP);
          }
          else{
            Serial.println("Error");
          }
      }
*/
  delay(80);
  /*
  if(millis()-start>100){
    digitalWrite(LED,!digitalRead(LED));
    Serial.println(millis());
    start = millis();
  }
  */
}
