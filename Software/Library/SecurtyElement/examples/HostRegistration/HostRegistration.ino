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
  Serial.println("Host Registration Demo!");
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
  if (buf[0] == 0x06) //DISCONN mode, login
  {
    Serial.println("NOHOST mode, Reg host");
    ret = BindReg();
    if (ret == -1)
    {
      Serial.println("BindReg Failed!");
      while (1);
    }
    else {
      Serial.println("BindReg Success");
    }
    ret = BindLogin();
    if (ret == -1)
    {
      Serial.println("BindLogin Failed!");
      while (1);
    }
    else {
      Serial.println("BindLogin Success");
    }

    Serial.println("Enable PinCode?(y/n)");
    while (1) {
      while (Serial.available()) {
        char temp = Serial.read();
        if (temp == 'y')
        {
          Serial.print("Please enter Pincode:");
          uint8_t pinCode[6];
          uint8_t pinCode_count = 0;
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
          ret = SetSecurityPolicy(pinCode);
          if (ret == -1)
          {
            Serial.println("SetSecurityPolicy Failed!");
            while (1);
          }
          else {
            Serial.println("SetSecurityPolicy Success");
          }
        }
        if (temp == 'n')
        {
          break;
        }
      }
    }
  }
  else {
    Serial.println("Not in NOHOST mode");
  }

}

int SetSecurityPolicy(uint8_t* PIN)
{
  int ret = 0;
  uint8_t SECPO[BCDC_SECPO_LEN];
  uint8_t pinChlng[16];
  ret = hardware_wallet.GetSECPO(SECPO);
  if (ret != 0)
    return ret;

  //byte[0] :
  //BIT[0]: Trx signing OTP verification
  //BIT[1] : Trx signing button confirm
  //BIT[2] : PIN verification / Trx signing PIN verification
  //BIT[3] : PIN LOCK mechanism
  Serial.print("[SetSecurityPolicy] SECPO[0]:"); Serial.println(SECPO[0], BIN);

  //Enable PIN verification / Trx signing PIN verification
  SECPO[0] |= SECPO_MASK_PIN;
  ret = hardware_wallet.PIN_CHLNG(true, pinChlng);
  if (ret != 0)
    return ret;

  ret = hardware_wallet.SetSECPO(SECPO, PIN, pinChlng);
  if (ret != 0) {
    return ret;
  }


  ret = hardware_wallet.GetSECPO(SECPO);
  if (ret != 0)
    return ret;
  Serial.print("[SetSecurityPolicy] SECPO[0]:"); Serial.println(SECPO[0], BIN);


  return ret;
}
void loop(void) {

}

int BindReg(void)
{
  int ret = 0;
  uint8_t firstHost = 0;   //true or false
  uint8_t rcv_buf[1024] = { 0 };
  uint8_t brHandle[HANDLE_LEN] = { 0 };
  uint8_t otp[BIND_OTP_LEN] = { 0 };
  uint8_t chlng[BIND_CHLNG_LEN] = { 0 };

  ret = hardware_wallet.BindRegInit(firstHost, hardware_wallet.host_cred, hardware_wallet.host_desc, rcv_buf);
  //Wrong first flag, try again
  if ((ret != 0) && (rcv_buf[7] == 0x40)) {
    Serial.println("[BindRegInit] Wrong first flag, try again");
    firstHost = firstHost ? 0 : 1;
    ret = hardware_wallet.BindRegInit(firstHost, hardware_wallet.host_cred, hardware_wallet.host_desc, rcv_buf);
  }
  if (ret != 0) {
    Serial.println("[BindRegInit] Error!");
    return ret;
  }
  //hardware_wallet.dumpData("BindRegInit", rcv_buf, HANDLE_LEN + BIND_OTP_LEN);

  memcpy(brHandle, rcv_buf, HANDLE_LEN);
  memcpy(otp, rcv_buf + HANDLE_LEN, BIND_OTP_LEN);

  ret = hardware_wallet.BindRegChallenge(brHandle, chlng);
  if (ret != 0) {
    return ret;
  }
  //hardware_wallet.dumpData("BindRegChallenge", chlng, BIND_CHLNG_LEN);

  //need to save OTP key for login
  ret = hardware_wallet.BindRegFinish(hardware_wallet.host_cred, brHandle, otp, chlng, rcv_buf);
  if (ret != 0) {
    return ret;
  }
  else {
    Serial.println((char*)rcv_buf);
    hardware_wallet.dumpData("BindRegFinish", rcv_buf, 2);         //host_id, bind_state
    uint8_t Cred_buf[1 + BIND_OTP_LEN + BIND_CHLNG_LEN] = {0};
    memcpy(Cred_buf, rcv_buf, 1);
    memcpy(Cred_buf + 1, otp, BIND_OTP_LEN);
    memcpy(Cred_buf + 1 + BIND_OTP_LEN, chlng, BIND_CHLNG_LEN);
    hardware_wallet.dumpData("Cred_buf", Cred_buf, 1 + BIND_OTP_LEN + BIND_CHLNG_LEN);

    LFlash.write("SE", "CRED", LFLASH_RAW_DATA, (const uint8_t *)&Cred_buf, sizeof(Cred_buf));
    hardware_wallet.WriteCred(Cred_buf);

  }

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
    goto __func_end;
  }
  //hardware_wallet.dumpData("BindLoginChallenge", chlng, BIND_CHLNG_LEN);

  ret = hardware_wallet.BindLogin(cred_buf[0], hardware_wallet.host_cred, cred_buf + 1, chlng, rcv_buf);
  if (ret != 0) {
    goto __func_end;
  }
  Serial.println("[BindLogin] Login Success");

  //store login challenge
  memcpy(cred_buf + 1 + BIND_OTP_LEN, chlng, BIND_CHLNG_LEN);
  //hardware_wallet.dumpData("cred_buf", cred_buf, 1+BIND_OTP_LEN+BIND_CHLNG_LEN);
  LFlash.write("SE", "CRED", LFLASH_RAW_DATA, (const uint8_t *)&cred_buf, 1 + BIND_OTP_LEN + BIND_CHLNG_LEN);
  hardware_wallet.WriteCred(cred_buf);

  ret = hardware_wallet.GetModeState(rcv_buf);
  //hardware_wallet.dumpData("GetModeState", rcv_buf, 2);

  if (ret != 0) {
    goto __func_end;
  }

  //perso mode, set the perso data and then enter normal mode
  if (rcv_buf[0] == 1) {
    ret = hardware_wallet.perso_set_data(hardware_wallet.host_cred, cred_buf + 1, chlng, rcv_buf);
    if (ret != 0)
    {
      Serial.println("[BindLogin perso mode] perso_set_data error");
    }

    uint8_t pinCode[6];
    uint8_t pinCode_count = 0;
    Serial.print("[BindLogin perso mode] Please enter Pincode:");
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

    ret = hardware_wallet.perso_set_pin_n(pinCode);
    if (ret != 0)
    {
      Serial.println("[BindLogin perso mode] perso_set_pin_n Error");
    }
    else {
      Serial.println("[BindLogin perso mode] Pin Set!");
    }

    ret = hardware_wallet.perso_confirm(rcv_buf);
    if (ret != 0)
    {
      Serial.println("[BindLogin perso mode] perso_confirm Error");
    }
  }


__func_end:
  return ret;
}

