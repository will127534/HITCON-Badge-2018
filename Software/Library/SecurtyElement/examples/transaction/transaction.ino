#include <SPI.h>
#include <wallet.h>
#include <LWiFi.h>
#include "LFlash.h"
#include <Transaction.h>
#include <CryptoHelper.h>
#include <ByteConverter.h>
#include <vector>
#define SE_SS 10

wallet hardware_wallet;
Transaction trx;
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
  Serial.println("Transaction Demo!");
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

  hdw_query_account_key_info(0x00);

  ret = CreateETH_Trx();
  if (ret == -1)
  {
    Serial.println("CreateETH_Trx Failed!");
    while (1);
  }
  else {
    Serial.println("CreateETH_Trx Success");
  }

}


void loop(void) {

}

int hdw_query_account_key_info(uint8_t id)
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

int CreateETH_Trx(void)
{

  transaction_struct transaction_data;
  transaction_data.to = ByteConverter::charStrToBytes((const uint8_t *)"0x1e37af84b385e47b87f493ab187e6a764162a35a");
  transaction_data.value = ByteConverter::charStrToBytes((const uint8_t *)"0x016345785D8A0000"); //0.1 ETH
  transaction_data.gasPrice = ByteConverter::charStrToBytes((const uint8_t *)"0x0BA43B7400"); // 50 Gwei
  transaction_data.gasLimit = ByteConverter::charStrToBytes((const uint8_t *)"0x5208"); //21000
  transaction_data.nonce = ByteConverter::charStrToBytes((const uint8_t *)"0x00");
  transaction_data.data = ByteConverter::charStrToBytes((const uint8_t *)"0x00");

  uint32_t account = 0;
  uint32_t key = 0;
  Serial.print("Account:"); Serial.print(account); Serial.print("/Key_ID:"); Serial.println(key);
  Serial.print("To: 0x"); Serial.println(vector2string(transaction_data.to));
  Serial.print("Value: "); Serial.print(vector2string(transaction_data.value));
  Serial.print("gasPrice: "); Serial.print(vector2string(transaction_data.gasPrice));
  Serial.print("gasLimit: "); Serial.println(vector2string(transaction_data.gasLimit));
  Serial.print("Nonce: "); Serial.println(vector2string(transaction_data.nonce));
  Serial.print("Data: "); Serial.println(vector2string(transaction_data.data));


  Transaction trx;
  vector<uint8_t> rawTransaction = trx.GenerateSignature(transaction_data);

  int ret = 0;
  uint8_t inputID = 0;
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };

  for (int i = 0; i < 4; ++i)
  {
    acc_id[i] = account >> (i * 8);
  }

  uint8_t key_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  for (int i = 0; i < 4; ++i)
  {
    key_id[i] = key >> (i * 8);
  }

  uint8_t SECPO[BCDC_SECPO_LEN];
  uint8_t rcv_buf[1024] = { 0 };

  uint8_t infoID = 3;    //HDW_ACCINFO_BALNC 3
  uint8_t balance[HDW_ACC_BAL_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
                                     };
  ret = hardware_wallet.hdw_set_acc_info(infoID, acc_id, balance, rcv_buf);


  uint8_t outAddr[OUTADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                   0x00, 0x00
                                 };
  ret = hardware_wallet.hdw_prep_trx_sign(inputID, KeyChain, acc_id, key_id, balance, &rawTransaction[0], rcv_buf);
  if (ret != 0) {
    return ret;
  }

  ret = hardware_wallet.hdw_trx_sign_begin(balance, outAddr, rcv_buf);
  if (ret != 0) {
    return ret;
  }

  ret = hardware_wallet.GetSECPO(SECPO);
  if (ret != 0) {
    return ret;
  }
  Serial.print("SECPO[0]"); Serial.println(SECPO[0], BIN);
  if (SECPO[0] & SECPO_MASK_PIN) {
    int i;
    for (i = 0; i < 3; ++i) {
      uint8_t pinCode[6];
      uint8_t pinCode_count = 0;
      if (i > 0)
      {
        Serial.print("Retry:"); Serial.println(i);
      }
      Serial.print("[Transaction] Please enter Pin Code:");
      while (1) {
        char c = Serial.read();
        if (c == '\n' || c == '\r' && pinCode_count == 6)
        {
          Serial.println();
          break;
        }
        if (c <= 0x39 && c >= 0x30 && pinCode_count < 6) //numbers
        {
          Serial.write(c);
          pinCode[pinCode_count] = c;
          pinCode_count ++;
        }
        if (c == '\b' && pinCode_count > 0) //backspace
        {
          pinCode_count --;
          Serial.write(c);
        }
      }
      ret = hardware_wallet.hdw_trx_verify(pinCode, SECPO_MASK_PIN);
      if (ret == 0) {
        Serial.println("[Transaction] Pin Code Pass");
        break;
      }
      else {
        if (i == 2)
        {
          Serial.println("[Transaction] Pin Code Error, canceled transaction");
        }
      }
    }
  }
  else {
    Serial.println("SECPO[0] = 0, No pin code needed");
  }

  ret = hardware_wallet.hdw_trx_sign(0, rcv_buf);
  if (ret != 0) {
    return ret;
  }
  Serial.println("SIGNATURE data:");
  hardware_wallet.dumpData("SIGNATURE", rcv_buf, 65);

  uint8_t signature[65] = {0};
  memcpy(signature, rcv_buf, 65);

  vector<uint8_t> param = trx.RlpEncodeForRawTransaction(transaction_data, signature, signature[64]);
  ret = hardware_wallet.hdw_trx_sign_finish(rcv_buf);

  Serial.println(vector2string(param));
  return ret;
}


String vector2string(std::vector<uint8_t> VEC) {
  String s;
  for (int i = 0; i < VEC.size(); i++) {
    if (VEC[i] < 16) {
      s += "0";
    }
    s += String(VEC[i], 16);
  }
  return s;
}