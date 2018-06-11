//NFC
#include <Adafruit_PN532.h>
//BLE
#include "epddisplay.hpp"
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
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void init_hardware(){
  Serial.begin(115200);
  Serial.println();
  Serial.println("setup");
  delay(100);
  pinMode(3,OUTPUT);
  digitalWrite(3,HIGH);
  pinMode(A2,INPUT);
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



void loop()
{
  if(readButton()==0xA){
    setting_menu();
    main_menu();
  }
  if (NewBalanceFlag)
  {
    NewBalanceFlag = 0;
    main_menu();
  }
  Process_BLE();
}
