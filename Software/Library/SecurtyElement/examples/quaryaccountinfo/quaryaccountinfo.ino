#include <SPI.h>
#include <wallet.h>
#include <LWiFi.h>
#include "LFlash.h"

#define SE_SS 10

wallet hardware_wallet;
CryptoHelper cyp;

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
  Serial.println("Quary Account Info Demo!");
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


  ret = hdw_query_wallet_info();
  ret = hdw_query_account_info(0x00);
  ret = hdw_query_account_key_info(0x00);



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

static int hdw_query_account_info(uint8_t id)
{
  int ret = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t rcv_buf[128] = { 0 };
  acc_id[0] = id;

  ret = hardware_wallet.hdw_qry_acc_info(acc_id, rcv_buf);
  //HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_BAL_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_BAL_LEN
  hardware_wallet.dumpData("ACCOUNT NAME", rcv_buf, HDW_NAME_LEN);
  hardware_wallet.dumpData("ACCOUNT PURPOSE", rcv_buf + HDW_NAME_LEN, HDW_ACC_PURPOSE_LEN);
  hardware_wallet.dumpData("ACCOUNT COINTYPE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN, HDW_ACC_COINTYPE_LEN);
  hardware_wallet.dumpData("ACCOUNT BALANCE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN, HDW_ACC_BAL_LEN);
  //dumpdata("XCHS BALANCE", rcv_buf + HDW_ACC_INFO_ALL_LEN - HDW_ACC_BAL_LEN, HDW_ACC_BAL_LEN);
  return ret;
}

static int hdw_query_wallet_info(void)
{
  int ret = 0;
  uint8_t rcv_buf[64] = { 0 };

  ret = hardware_wallet.hdw_qry_wa_info(rcv_buf);
  if (ret != 0)
    return ret;
  Serial.print("hdw status 0x"); Serial.println(rcv_buf[0], HEX);
  hardware_wallet.dumpData("hdw name:", rcv_buf + 1, HDW_NAME_LEN);
  hardware_wallet.dumpData("hdw account ptr:", rcv_buf + 1 + HDW_NAME_LEN, 4);

  return ret;
}
static int hdw_query_account_key_info(uint8_t id)
{
  int ret = 0;
  uint8_t infoID[3] = { HDW_ACCKINFO_ADDR , HDW_ACCKINFO_KPUB , HDW_ACCKINFO_ACCKPUB };
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t key_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t rcv_buf[128] = { 0 };
  acc_id[0] = id;

  ret = hardware_wallet.hdw_qry_acc_key_info(infoID[1], KeyChain, acc_id, key_id, rcv_buf);
  if (ret == 0) {
    hardware_wallet.dumpData("pub key", rcv_buf, HDW_KPUB_LEN);
  }

  uint8_t publickey[HDW_KPUB_LEN] = {0};
  memcpy(publickey, rcv_buf, HDW_KPUB_LEN);

  vector<uint8_t> address = cyp.generateAddress(publickey);

  Serial.println("Address: ");

  for (int i = 0; i < address.size(); ++i)
  {
    if (address[i] <= 0xF)
    {
      Serial.print("0");
    }
    Serial.print(address[i], HEX);
  }
  Serial.println();

  return ret;
}
static int hdw_generate_next_trx_addr(uint8_t id)
{
  int ret = 0;
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t rcv_buf[64] = { 0 };
  acc_id[0] = id;
  //for account 0 BTC
  ret = hardware_wallet.hdw_next_trx_addr(KeyChain, acc_id, rcv_buf);
  hardware_wallet.dumpData("Addr", rcv_buf + HDW_ACC_ID_LEN, BC_BINADDR_LEN);

  return ret;
}

