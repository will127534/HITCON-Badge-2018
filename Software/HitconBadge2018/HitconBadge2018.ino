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



//Print and converting String etc...
#include "util.hpp"
//transaction related
#include "wallet.hpp"
//saving the data
#include "LFlash.h"
//NFC
#define PN532_IRQ   (2)
#define PN532_RESET (17)  
#define LED (A0)  
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void init_hardware(){
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);
  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
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
}

void setup(void)
{
  init_hardware();
  init_se();
  init_display();
  init_BLE();
  main_menu();
}

bool BT_icon_updated=0;
bool BT_icon_updated2=0;
uint32_t start = 0;
uint32_t lastupdate_vbat = 0;
uint8_t battery_pattial,balance_partial,BTicon_partial = 0;

void loop()
{
  if(readButton()==0xA){
    setting_menu();
    main_menu();
  }
  if(readButton()==0xB){
    
  }
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
    Serial.println(readBatteryVoltage(),3);
    display.fillRect(190, 0, 59, 30,GxEPD_WHITE);
    display.setFont(&Roboto_Medium8pt7b);
    display.setCursor(190, 15);
    display.print(readBatteryVoltage(),3);
    display.println(" V");
    display.updateWindow(190, 0, 59, 15, true);
    display.powerDown();
    battery_pattial++;
  }
  if (BTicon_partial>10 || balance_partial>5 || battery_pattial>5)
  {
    main_menu();
    battery_pattial=0;
    balance_partial=0;
    BTicon_partial=0;

  }
  Process_BLE();
  delay(50);
  /*
  if(millis()-start>100){
    digitalWrite(LED,!digitalRead(LED));
    Serial.println(millis());
    start = millis();
  }
  */
}
