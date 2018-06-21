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
  if(Serial.available()){
    char c = Serial.read();
    if(c=='A'){
      setting_menu();
      main_menu();
    }
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

    display.update();
    balance_partial++;

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
