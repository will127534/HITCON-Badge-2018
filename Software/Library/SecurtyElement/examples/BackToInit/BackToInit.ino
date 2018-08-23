#include <SPI.h>
#include <wallet.h>
#include <LWiFi.h>
#include "LFlash.h"

#define SE_SS 10

wallet hardware_wallet;

extern uint8_t vmk_default[VMK_LEN];
extern uint8_t vmk_new[VMK_LEN];
extern uint8_t PIN_DEF[6];
extern uint8_t PIN_UNLOCK[6];


void setup(void) {


  Serial.begin(115200);

  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  Serial.print(mac[5], HEX);
  Serial.print(":");
  Serial.print(mac[4], HEX);
  Serial.print(":");
  Serial.print(mac[3], HEX);
  Serial.print(":");
  Serial.print(mac[2], HEX);
  Serial.print(":");
  Serial.print(mac[1], HEX);
  Serial.print(":");
  Serial.println(mac[0], HEX);

  uint8_t host_cred[32] = {0};
  memcpy(hardware_wallet.host_cred, mac, 6);

  if (LFlash.begin() != LFLASH_OK)
  {
    Serial.println("Flash init failed.");
    return;
  }

  uint32_t size =  64;
  uint8_t Cred_buf[64] = {0};
  LFlash.read("SE", "CRED", (uint8_t *)&Cred_buf, &size);
  hardware_wallet.WriteCred(Cred_buf);
  hardware_wallet.init(SE_SS);
  Serial.println("Back To Init Demo!");
  waitforcmd();


  uint8_t buf[32] = {0};
  int ret = 0;

  ret = hardware_wallet.loader_cmdhdlr_KvVersion(buf);
  hardware_wallet.dumpData("Version", buf, 16);

  uint8_t Bootloader[] = {0xd0, 0xa5, 0x59, 0x51};
  uint8_t MainCode[] = {0xd0, 0xa5, 0xe3, 0xb0};

  if (memcmp(buf, Bootloader, 4) == 0) // bootloader state
  {
    delay(10);
    Serial.println("Bootloader, Jump to main code");
    ret = hardware_wallet.loader_cmdhdlr_CosJump();
    if (ret == -1)
    {
      Serial.println("loader_cmdhdlr_CosJump Failed!");
      while (1);
    }
    else {
      Serial.println("loader_cmdhdlr_CosJump Success");
    }
  }
  waitforcmd();
  ret = hardware_wallet.GetModeState(buf);
  if (ret == -1)
  {
    Serial.println("GetModeState Failed!");
    while (1);
  }
  else {
    Serial.println("GetModeState Success");
  }
  Serial.print("Mode:"); Serial.println(buf[0], HEX);
  Serial.print("State:"); Serial.println(buf[1], HEX);

  waitforcmd();
  if (buf[0] == 0x02) //Normal Mode
  {
    Serial.println("[Re-init SE] Normal Mode, SE logut");
    hardware_wallet.BindLogout();

    ret = hardware_wallet.GetModeState(buf);
    if (ret == -1)
    {
      Serial.println("GetModeState Failed!");
      while (1);
    }
    else {
      Serial.println("GetModeState Success");
    }
    Serial.print("Mode:"); Serial.println(buf[0], HEX);
    Serial.print("State:"); Serial.println(buf[1], HEX);
  }

  if (buf[0] == 0x07) //DISCONN mode, login
  {
    Serial.println("DISCONN mode, Clear Wallet(BackToInit)");
    waitforcmd();
    ret = BackToInit();
    if (ret == -1)
    {
      Serial.println("BackToInit Failed!");
      while (1);
    }
    else {
      Serial.println("BackToInit Success");
    }
  }
  else {
    Serial.println("Not in DISCONN mode");
  }

  ret = hardware_wallet.GetModeState(buf);
  if (ret == -1)
  {
    Serial.println("GetModeState Failed!");
    while (1);
  }
  else {
    Serial.println("GetModeState Success");
  }
  Serial.print("Mode:"); Serial.println(buf[0], HEX);
  Serial.print("State:"); Serial.println(buf[1], HEX);
}


void loop(void) {

}
void waitforcmd() {
  Serial.println("Wait for User input...");
  while (!Serial.available()) {
    delay(100);
  }
  while (Serial.available()) {
    char temp = Serial.read();
  }
}

int BackToInit(void)
{
  int ret = 0;
  uint8_t chlng[AUTHUTIL_CHLNG_LEN];
  //uint8_t *curVMK;
  //curVMK = vmk_new;

  ret = hardware_wallet.InitVMK_CHLNG(chlng);
  if (ret != 0)
    return ret;
  //vmk default 0x40~0x5F
  for (int i = 0; i < VMK_LEN; ++i) {
    vmk_default[i] = 0x40 + i;
  }

  ret = hardware_wallet.InitBackInit(chlng, vmk_default);
  if (ret != 0)
    return ret;

  return ret;
}