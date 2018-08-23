#include <SPI.h>
#include <wallet.h>
#include <LWiFi.h>
#include "LFlash.h"

#define SE_SS 10

wallet hardware_wallet;


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
  Serial.println("Host Login Demo!");
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

  if (buf[0] == 0x07) //DISCONN mode, login
  {
    Serial.println("DISCONN mode, logging in");
    ret = BindLogin();
    if (ret == -1)
    {
      Serial.println("BindLogin Failed!");
      while (1);
    }
    else {
      Serial.println("BindLogin Success");
    }
  }
  else {
    Serial.println("Not in DISCONN mode");
  }

  Serial.println("InitWalletFromMnemonic");
  ret = InitWalletFromMnemonic();
  if (ret == -1)
  {
    Serial.println("InitWalletFromMnemonic Failed!");
    while (1);
  }
  else {
    Serial.println("InitWalletFromMnemonic Success");
  }

}


void loop(void) {
  delay(1000000);
}

int BindLogin()
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t chlng[BIND_CHLNG_LEN] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };

  uint32_t size =  64;
  LFlash.read("SE", "CRED", (uint8_t *)&cred_buf, &size);

  //hardware_wallet.dumpData("credential", cred_buf,size);
  //hardware_wallet.ReadCred(cred_buf);
  //hardware_wallet.dumpData("credential", cred_buf, 1 + BIND_OTP_LEN);

  ret = hardware_wallet.BindLoginChallenge(cred_buf[0], chlng);
  if (ret != 0) {
    return ret;
  }
  //hardware_wallet.dumpData("BindLoginChallenge", chlng, BIND_CHLNG_LEN);

  ret = hardware_wallet.BindLogin(cred_buf[0], hardware_wallet.host_cred, cred_buf + 1, chlng, rcv_buf);
  if (ret != 0) {
    return ret;
  }
  Serial.println("[BindLogin] Login Success");

  //store login challenge
  memcpy(cred_buf + 1 + BIND_OTP_LEN, chlng, BIND_CHLNG_LEN);
  //hardware_wallet.dumpData("cred_buf", cred_buf, 1+BIND_OTP_LEN+BIND_CHLNG_LEN);
  LFlash.write("SE", "CRED", LFLASH_RAW_DATA, (const uint8_t *)&cred_buf, 1 + BIND_OTP_LEN + BIND_CHLNG_LEN);
  hardware_wallet.WriteCred(cred_buf);

  return ret;
}

void waitforcmd() {
  Serial.println("Enter to continue...");
  while (!Serial.available()) {
    delay(100);
  }
  while (Serial.available()) {
    char temp = Serial.read();
  }
}

int InitWalletFromMnemonic(void)
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };

  char *hdw_name = "ikv-tech_RaymondLiou779876543210";  //HDW_NAME_LEN
  char *passphrase = "RaymondLiou";
  char *mnemonic   = "uphold daring answer grief noble morning slim talent faint foster melt company young track popular chapter title oven absurd tuition shop luxury settle always";
  char *mnemonic_w = "uphold daring answer grief noble morning slim talent faint aoster melt company young track popular chapter title oven absurd tuition shop luxury settle always";

  //seed0 : mnemonic /wo  passphrase
  Serial.println("mnemonic /wo  passphrase, please wait for 1 min");
  ret = hardware_wallet.hdw_init_wallet_words((uint8_t *)hdw_name, NULL, 0, mnemonic, strlen(mnemonic), rcv_buf);
  /*
  //seed1 : wrong mnemonic /w passphrase
  Serial.println("wrong mnemonic /w passphrase");
  ret = hardware_wallet.hdw_init_wallet_words((uint8_t *)hdw_name, passphrase, strlen(passphrase), mnemonic_w, strlen(mnemonic_w), rcv_buf);

  //seed1: passphrase
  //ret = hdw_init_wallet_words((uint8_t *)hdw_name, passphrase, strlen(passphrase), NULL, 0, rcv_buf);
  Serial.println("passphrase");
  ret = hardware_wallet.hdw_init_wallet_words((uint8_t *)hdw_name, passphrase, strlen(passphrase), mnemonic, strlen(mnemonic), rcv_buf);

  //seed0 : mnemonic /w  passphrase
  //ret = hdw_init_wallet_words((uint8_t *)hdw_name, passphrase, strlen(passphrase), mnemonic, strlen(mnemonic), rcv_buf);
  */
  return ret;
}