#include "wallet.hpp"
#include "CryptoHelper.h"
#include <Transaction.h>
#include <wallet.h>
#include "Arduino.h"
#include "BLE.hpp"
#include "LFlash.h"
#include "util.hpp"
#include <nvdm.h>
#include <LWiFi.h>
#include "epddisplay.hpp"


std::vector<erc20_struct> erc20s;

uint8_t hitconToken_address[] = {0x50,0x3b,0x0C,0x13,0x96,0x65,0xE7,0xe9,0xF8,0x63,0xba,0x1B,0xCf,0x0e,0x63,0x5a,0x2E,0x87,0xaA,0x5b};
char hitconToken_Name[] = "HTN";

char* SE_Mode[] = {"INIT","PERSO","NORMAL","AUTH","LOCK","ERROR","NOHOST","DISCONN"};

String publicAddress_string;

wallet hardware_wallet;

//jungle enjoy into abandon abandon about essay effort glass total charge aspect punch drastic drive muffin demand winner 
//walk pilot aim mean recall code panther patch choice glass transfer accuse slush blouse income helmet spatial mean
//weird end social slogan nice repair neglect thought charge door robot solar mercy grant scheme sheriff drive silk  
void init_se(){
    byte mac[6];
    int ret = 0;
    WiFi.macAddress(mac);
    uint8_t host_cred[32] = {0};
    memcpy(hardware_wallet.host_cred,mac,6);
    //hardware_wallet.dumpData("host_cred", hardware_wallet.host_cred, 32);

    uint32_t size =  64;
    uint8_t Cred_buf[64] = {0}; 
    LFlash.read("SE","CRED",(uint8_t *)&Cred_buf,&size);
    hardware_wallet.WriteCred(Cred_buf);

    
    
    delay(10);
    uint8_t buf[32] = {0}; 

    int retry = 3;

    while(retry){
        ret = hardware_wallet.loader_cmdhdlr_KvVersion(buf);
        hardware_wallet.dumpData("Version",buf,16);
        if (ret == -1)
        {
            Serial.println("[init Secure Element] GetVersion Failed!");
            if(retry==1){
              secureelement_error();
            }
        }
        else{
            Serial.println("[init Secure Element] GetVersion Success");
            break;
        }
        retry--;
    }

    
    hardware_wallet.dumpData("Secure Element Version", buf, 16);

    uint8_t Bootloader[] = {0xd0,0xa5,0x59,0x51};
    uint8_t MainCode[] = {0xd0,0xa5,0xe3,0xb0};

    if (memcmp(buf,Bootloader,4) == 0) // bootloader state
    {
        delay(10);
        Serial.println("[init Secure Element] Bootloader, Jump to main code");
        ret = hardware_wallet.loader_cmdhdlr_CosJump();
        if (ret == -1)
        {
            Serial.println("[init Secure Element] loader_cmdhdlr_CosJump Failed!");
            secureelement_error();
        }
        else{
            Serial.println("[init Secure Element] loader_cmdhdlr_CosJump Success");
        }
    }
    
    ret = hardware_wallet.GetModeState(buf);
    if (ret == -1)
    {
        Serial.println("[init Secure Element] GetModeState Failed!");
        secureelement_error();
    }
    else{
        Serial.println("[init Secure Element] GetModeState Success");
    }
    Serial.print("[init Secure Element] Mode:");Serial.println(buf[0],HEX);
    Serial.print("[init Secure Element] State:");Serial.println(buf[1],HEX);

    if (buf[0] == 0x00) //INIT mode, init SE
    {
        Serial.println("[init Secure Element] INIT mode");
        InitDevInfo();
        ReinitWallet();
    }
    if (buf[0] == 0x06) //no host,init wallet
    {
        Serial.println("[init Secure Element] NOHOST mode, re-Regester & initialize the wallet");
        ReinitWallet();
    }

    if (buf[0] == 0x01) //perso mode, should not be in this mode, logout and reinit
    {
        Serial.println("[init Secure Element] perso mode, logging in");
        ret = BindLogout();
        if (ret == -1)
        {
            Serial.println("[init Secure Element] BindLogin Failed!");
            secureelement_error();
        }
        else{
            Serial.println("[init Secure Element] BindLogin Success");
        }
        ReinitWallet();
    }

    if (buf[0] == 0x02) //Logged in already, do nothing
    {
        Serial.println("[init Secure Element] NORMAL mode");
    }

    if (buf[0] == 0x07) //DISCONN mode, login
    {
        Serial.println("[init Secure Element] DISCONN mode, logging in");
        ret = BindLogin(NULL);
        if (ret == -1)
        {
            Serial.println("[init Secure Element] BindLogin Failed!");
            secureelement_error();
        }
        else{
            Serial.println("[init Secure Element] BindLogin Success");
        }
    }

    retry = 3;
    while(retry){
      Serial.println("[init Secure Element] Query Account Public Key");
      ret = hdw_query_account_key_info(0x00);
      if (ret != 0)
      {
          Serial.println("[init Secure Element] hdw_query_account_key_info Failed!");
          if(retry==1){
            ReinitWallet();
          }
      }
      else{
          Serial.println("[init Secure Element] hdw_query_account_key_info Success");
          break;
      }
      retry--;
    }
    
    Serial.print("[init Secure Element] Wallet Address:");
    Serial.print(publicAddress_string);
    Serial.println();


    quary_erc20s();
    
}

int BindLogin(uint8_t* new_pin)
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t chlng[BIND_CHLNG_LEN] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };

  uint32_t size =  64;
  LFlash.read("SE","CRED",(uint8_t *)&cred_buf,&size);

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
  memcpy(cred_buf+1+BIND_OTP_LEN, chlng, BIND_CHLNG_LEN);
  //hardware_wallet.dumpData("cred_buf", cred_buf, 1+BIND_OTP_LEN+BIND_CHLNG_LEN); 
  LFlash.write("SE","CRED",LFLASH_RAW_DATA,(const uint8_t *)&cred_buf,1+BIND_OTP_LEN+BIND_CHLNG_LEN);  
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

        ret = hardware_wallet.perso_set_pin_n(new_pin);
        if (ret != 0)
        {
            Serial.println("[BindLogin perso mode] perso_set_pin_n Error");
        }
        else{
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
extern uint8_t vmk_default[VMK_LEN];
extern uint8_t vmk_new[VMK_LEN];
extern uint8_t PIN_DEF[6];
extern uint8_t PIN_UNLOCK[6];
int InitDevInfo(void)
{
    int ret = 0;

    uint8_t CARDID[8] = { 0x33, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x3a };
    uint8_t DUMMY[6] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t HASH[32] = { 0 };

    ret = hardware_wallet.SetInitData(BCDC_IDATA_UPINHASH, 0, PIN_DEF, sizeof(PIN_DEF));
    if (ret != 0) {
        return ret;
    }

    ret = hardware_wallet.SetInitData(BCDC_IDATA_PUK, 0, PIN_UNLOCK, sizeof(PIN_UNLOCK));
    if (ret != 0) {
        return ret;
    }

    ret = hardware_wallet.SetInitData(BCDC_IDATA_CARDID, 0, CARDID, sizeof(CARDID));
    if (ret != 0) {
        return ret;
    }

    //ID 2, 4, 5 will be obsoleted
    ret = hardware_wallet.SetInitData(2, 0, DUMMY, sizeof(DUMMY));
    if (ret != 0) {
        return ret;
    }
    ret = hardware_wallet.SetInitData(4, 0, DUMMY, sizeof(DUMMY));
    if (ret != 0) {
        return ret;
    }
    ret = hardware_wallet.SetInitData(5, 0, DUMMY, sizeof(DUMMY));
    if (ret != 0) {
        return ret;
    }

    ret = hardware_wallet.InitDataConfirm();
    if (ret != 0) {
            return ret;
    }

    return ret;
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
    Serial.print("[SetSecurityPolicy] SECPO[0]:");Serial.println(SECPO[0],BIN);

    //Enable PIN verification / Trx signing PIN verification
    SECPO[0] |= SECPO_MASK_PIN;
    ret = hardware_wallet.PIN_CHLNG(true, pinChlng);
    if (ret != 0)
        return ret;

    ret = hardware_wallet.SetSECPO(SECPO, PIN, pinChlng);
    if (ret != 0){
        return ret;
    }
   

    ret = hardware_wallet.GetSECPO(SECPO);
    if (ret != 0)
        return ret;
    Serial.print("[SetSecurityPolicy] SECPO[0]:");Serial.println(SECPO[0],BIN);


    return ret;
}

int BindLogout(void)
{
    hardware_wallet.BindLogout();
}

int BackToNoHost(uint8_t *PIN)
{
    int ret = 0;
    uint8_t chlng[AUTHUTIL_CHLNG_LEN];
    ret = hardware_wallet.PIN_CHLNG(false, chlng);
    if (ret != 0)
        return ret;
    //if in login state, the 2nd parameter should not be NULL
    ret = hardware_wallet.BindBackToNoHost(PIN, chlng);
    if (ret != 0)
        return ret;
    return 0;
}

int PIN_AuthTest(uint8_t *inputPIN)
{
    int ret = 0;
    uint8_t rcv_buf[16];
    hardware_wallet.dumpData("inputPIN", inputPIN, 6);

    ret = hardware_wallet.PIN_CHLNG(true, rcv_buf);
    if (ret != 0)
        return ret;

    ret = hardware_wallet.ModeAuth(inputPIN ,rcv_buf);
    if (ret != 0)
        return ret;

    return ret;
}
int BackToInit(void)
{
  int ret = 0;
  uint8_t chlng[AUTHUTIL_CHLNG_LEN];
 // uint8_t *curVMK;

 // curVMK = vmk_new;

  ret = hardware_wallet.InitVMK_CHLNG(chlng);
  if (ret != 0)
    return ret;

  Serial.println("InitBackInit");

    //vmk default 0x40~0x5F
    for (int i = 0; i < VMK_LEN; ++i) {
        vmk_default[i] = 0x40 + i;
    }

  ret = hardware_wallet.InitBackInit(chlng, vmk_default);
  if (ret != 0)
    return ret;

  return ret;
}
int InitWalletFromMnemonic(String mnemonic)
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };
  
  char *hdw_name = "Hitcon Badge 2018";  //HDW_NAME_LEN
  //seed: mnemonic /wo  passphrase
  Serial.println("[InitWalletFromMnemonic] mnemonic /wo  passphrase");
  Serial.println("[InitWalletFromMnemonic] Wait for 1 min");
  ret = hardware_wallet.hdw_init_wallet_words((uint8_t *)hdw_name, NULL, 0,(char*)mnemonic.c_str(), strlen( mnemonic.c_str()), rcv_buf);
  return ret;
}
int InitWalletFromMnemonic_withpassphrase(String mnemonic,String passphrase)
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };
  
  char *hdw_name = "Hitcon Badge 2018";  //HDW_NAME_LEN
  //seed: mnemonic /wo  passphrase
  Serial.println("[InitWalletFromMnemonic] mnemonic /w  passphrase");
  Serial.println("[InitWalletFromMnemonic] Wait for 1 min");
  ret = hardware_wallet.hdw_init_wallet_words((uint8_t *)hdw_name,(char*)passphrase.c_str(), strlen( passphrase.c_str()),(char*)mnemonic.c_str(), strlen( mnemonic.c_str()), rcv_buf);
  return ret;
}
int hdw_query_wallet_info(void)
{
  int ret = 0;
  uint8_t rcv_buf[64] = { 0 };

  ret = hardware_wallet.hdw_qry_wa_info(rcv_buf);
  if (ret != 0)
    return ret;
  Serial.print("hdw status 0x");Serial.println(rcv_buf[0],HEX);
  hardware_wallet.dumpData("hdw name:", rcv_buf + 1, HDW_NAME_LEN);
  hardware_wallet.dumpData("hdw account ptr:", rcv_buf + 1 + HDW_NAME_LEN, 4);

  return ret;
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
  vector<uint8_t>publicAddress =  CryptoHelper::generateAddress(rcv_buf);
  publicAddress_string = vector2string(publicAddress);
  return ret;
}
int hdw_query_account_key_info(uint32_t account,uint32_t key)
{
  int ret = 0;
  uint8_t infoID[3] = { HDW_ACCKINFO_ADDR , HDW_ACCKINFO_KPUB , HDW_ACCKINFO_ACCKPUB };
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t key_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t rcv_buf[128] = { 0 };

  for (int i = 0; i < 4; ++i)
  {
      acc_id[i] = account >> (i*8);
  }
  for (int i = 0; i < 4; ++i)
  {
      key_id[i] = key >> (i*8);
  }
  Serial.print("Query account id: ");Serial.print(account);Serial.print(" Key_id:");Serial.println(key);

  ret = hardware_wallet.hdw_qry_acc_key_info(infoID[1], KeyChain, acc_id, key_id, rcv_buf);
  if (ret == 0) {
    hardware_wallet.dumpData("pub key", rcv_buf, HDW_KPUB_LEN);
    vector<uint8_t>publicAddress =  CryptoHelper::generateAddress(rcv_buf);
    Serial.print("ETH Address:");Serial.println(vector2string(publicAddress));
  }

  return ret;
}
int hdw_create_account(uint8_t id)
{
  int ret = 0;
  uint8_t rcv_buf[64] = { 0 };
  uint8_t acc_name[HDW_ACCNAME_LEN] = { 0 };
  uint8_t acc_purpose[HDW_ACC_PURPOSE_LEN] = { 0x2c, 0x00, 0x00, 0x80 };      //44
  uint8_t acc_cointype[HDW_ACC_COINTYPE_LEN] = { 0x01, 0x00, 0x00, 0x80 };    //bitcoin testnet
  acc_cointype[0] = 0x3c; //ETH
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  char *tmp = "eth_test";
  acc_id[0] = id;
  memcpy(acc_name, (uint8_t *)tmp, strlen(tmp));
  ret = hardware_wallet.hdw_create_account(acc_name, acc_purpose, acc_cointype, acc_id, rcv_buf);
  return ret;
}
int hdw_create_account(uint32_t cointype,uint32_t account,char* account_name)
{
  int ret = 0;
  uint8_t rcv_buf[64] = { 0 };
  uint8_t acc_name[HDW_ACCNAME_LEN] = { 0 };
  uint8_t acc_purpose[HDW_ACC_PURPOSE_LEN] = { 0x2c, 0x00, 0x00, 0x80 };      //44
  uint8_t acc_cointype[HDW_ACC_COINTYPE_LEN] = { 0x00, 0x00, 0x00, 0x80 };    
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  char *tmp = account_name;

  for (int i = 0; i < 4; ++i)
  {
      acc_cointype[i] = cointype >> (i*8);
  }
  for (int i = 0; i < 4; ++i)
  {
      acc_id[i] = account >> (i*8);
  }
  Serial.print("Creating account: m/44'/");Serial.print(ArraytoString(acc_cointype,4));Serial.print("/");Serial.println(ArraytoString(acc_id,4));

  memcpy(acc_name, (uint8_t *)tmp, strlen(tmp));
  ret = hardware_wallet.hdw_create_account(acc_name, acc_purpose, acc_cointype, acc_id, rcv_buf);
  return ret;
}
int hdw_query_all_account_info(void)
{
    int ret = 0;
    int i;
    uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
    uint8_t rcv_buf[128] = { 0 };

    //query all account
    for (i = 0; ; ++i) {
        acc_id[0] = i;
        ret = hardware_wallet.hdw_qry_acc_info(acc_id, rcv_buf);
        if (ret != 0){
            Serial.println("Query End!");
            break;
        }

        hardware_wallet.dumpData("ACCOUNT ID", acc_id, HDW_ACC_ID_LEN);
        hardware_wallet.dumpData("ACCOUNT PURPOSE", rcv_buf + HDW_NAME_LEN, HDW_ACC_PURPOSE_LEN);
        hardware_wallet.dumpData("ACCOUNT COINTYPE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN, HDW_ACC_COINTYPE_LEN);
        hardware_wallet.dumpData("ACCOUNT BALANCE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN, HDW_ACC_BAL_LEN);
        Serial.println("");
    }
    return 1;
}
int hdw_query_account_info(uint8_t id)
{
  int ret = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  acc_id[0] = id;
  uint8_t rcv_buf[128] = { 0 };

  ret = hardware_wallet.hdw_qry_acc_info(acc_id, rcv_buf);
  //HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_BAL_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_BAL_LEN
  hardware_wallet.dumpData("ACCOUNT NAME", rcv_buf, HDW_NAME_LEN);
  hardware_wallet.dumpData("ACCOUNT PURPOSE", rcv_buf + HDW_NAME_LEN, HDW_ACC_PURPOSE_LEN);
  hardware_wallet.dumpData("ACCOUNT COINTYPE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN, HDW_ACC_COINTYPE_LEN);
  hardware_wallet.dumpData("ACCOUNT BALANCE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN, HDW_ACC_BAL_LEN);
  //dumpdata("XCHS BALANCE", rcv_buf + HDW_ACC_INFO_ALL_LEN - HDW_ACC_BAL_LEN, HDW_ACC_BAL_LEN);
  return ret;
}
int hdw_query_account_info_cmdline(uint32_t account)
{
  int ret = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  for (int i = 0; i < 4; ++i)
  {
      acc_id[i] = account >> (i*8);
  }
  uint8_t rcv_buf[128] = { 0 };

  ret = hardware_wallet.hdw_qry_acc_info(acc_id, rcv_buf);
  //HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_BAL_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_BAL_LEN
  hardware_wallet.dumpData("ACCOUNT NAME", rcv_buf, HDW_NAME_LEN);
  hardware_wallet.dumpData("ACCOUNT PURPOSE", rcv_buf + HDW_NAME_LEN, HDW_ACC_PURPOSE_LEN);
  hardware_wallet.dumpData("ACCOUNT COINTYPE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN, HDW_ACC_COINTYPE_LEN);
  hardware_wallet.dumpData("ACCOUNT BALANCE", rcv_buf + HDW_NAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN, HDW_ACC_BAL_LEN);
  //dumpdata("XCHS BALANCE", rcv_buf + HDW_ACC_INFO_ALL_LEN - HDW_ACC_BAL_LEN, HDW_ACC_BAL_LEN);
  return ret;
}
int hdw_generate_next_trx_addr(uint8_t id)
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
int hdw_generate_next_trx_addr_account(uint32_t account)
{
  int ret = 0;
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t rcv_buf[64] = { 0 };

  for (int i = 0; i < 4; ++i)
  {
      acc_id[i] = account >> (i*8);
  }
  //for account 0 BTC
  ret = hardware_wallet.hdw_next_trx_addr(KeyChain, acc_id, rcv_buf);
  hardware_wallet.dumpData("Addr", rcv_buf + HDW_ACC_ID_LEN, BC_BINADDR_LEN);

  return ret;
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
  if (ret != 0){
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
    uint8_t Cred_buf[1+BIND_OTP_LEN+BIND_CHLNG_LEN] = {0};
    memcpy(Cred_buf, rcv_buf, 1);
    memcpy(Cred_buf+ 1, otp, BIND_OTP_LEN);
    memcpy(Cred_buf+ 1+BIND_OTP_LEN, chlng, BIND_CHLNG_LEN);
    hardware_wallet.dumpData("Cred_buf", Cred_buf, 1+BIND_OTP_LEN+BIND_CHLNG_LEN); 
    
    LFlash.write("SE","CRED",LFLASH_RAW_DATA,(const uint8_t *)&Cred_buf,sizeof(Cred_buf));  
    hardware_wallet.WriteCred(Cred_buf);

  }

  return ret;
}
void reinit_secureelement(String Memonic){
    int ret = 0;
    uint8_t buf[2] = {0}; 

    int retry = 3;
    while(retry){
        ret = hardware_wallet.GetModeState(buf);
        if (ret ==0 )
        {
            break;
        }
    }

    if (ret == -1)
    {
        Serial.println("[init Secure Element] GetModeState Failed!");
        secureelement_error();
    }
    else{
        Serial.println("[init Secure Element] GetModeState Success");
    }
    Serial.print("[Re-init SE] Mode:");Serial.print(buf[0],HEX);Serial.print("==>");Serial.println(SE_Mode[buf[0]]);
    Serial.print("[Re-init SE] State:");Serial.println(buf[1],HEX);

    if (buf[0] == 0x02) //Normal Mode
    {
        Serial.println("[Re-init SE] Normal Mode, SE logut");
        BindLogout();
    }


    if (buf[0] == 0x07 || buf[0] == 0x02) //Disconnect mode ==> BackToInit
    {
        Serial.println("[Re-init SE] Clear Wallet(BackToInit)");

        ret = BackToInit();
        delay(100);
        

        int retry = 3;
        while(retry>0){
            ret = hardware_wallet.GetModeState(buf);
            if (ret == -1)
            {
                Serial.println("[Re-init SE] GetModeState Failed!");
                if (retry == 1)
                {
                    secureelement_error();
                }
            }
            retry --;
            delay(100);
        }
        InitDevInfo();

        retry = 3;
        while(retry>0){
            ret = hardware_wallet.GetModeState(buf);
            if (ret == -1)
            {
                Serial.println("[Re-init SE] GetModeState Failed!");
                if (retry == 1)
                {
                    secureelement_error();
                }
            }
            retry --;
            delay(100);
        }
    }

    Serial.print("Mode:");Serial.println(SE_Mode[buf[0]]);
    if (buf[0] != 0x06)
    {
        Serial.println("Not in NOHOST mode, Stop the reinit procedure");
        secureelement_error();
    }

    Serial.println("[Re-init SE] Bind Reg");
    ret = BindReg();
    if (ret != 0)
    {
        Serial.print("[Re-init SE] BindReg Failed! Stop the reinit procedure");
        secureelement_error();
    }

    uint8_t new_pin[6] = {0x30,0x30,0x30,0x30,0x30,0x30};
    bool lock = 1;
    while(lock){
        setpincode_page(new_pin);
        uint8_t re_new_pin[6] = {0x30,0x30,0x30,0x30,0x30,0x30};
        for (int i = 0; i < 3; ++i)
        {
            confirm_pincode_page(re_new_pin,i);
            if (memcmp(re_new_pin,new_pin,6) == 0)
            {
                pincode_success();
                lock = 0; 
                break;
            }
        }
        memset(re_new_pin,0,6);
    }
    uint8_t new_pin_2[6] = {0x30,0x30,0x30,0x30,0x30,0x30};
    memcpy(new_pin_2,new_pin,6);


    digitalWrite(A0,HIGH);

    retry = 3;
    while(retry){
        Serial.println("[Re-init SE] Bind Login");
        ret = BindLogin(new_pin);
        if (ret != 0)
        {
            Serial.print("[Re-init SE] BindLogin Failed! Stop the reinit procedure");
            if(retry==1){
              secureelement_error();
            }
        }
        else{
            Serial.print("[Re-init SE] BindLogin Success!");
            break;
        }
        retry --;
    }


    Serial.println("[Re-init SE] Set Security Policy");
    ret = SetSecurityPolicy(new_pin_2);
    if (ret != 0)
    {
        Serial.print("[Re-init SE] SetSecurityPolicy Failed! Stop the reinit procedure");
        secureelement_error();
    }
    else{
        Serial.println("[Re-init SE] SetSecurityPolicy Success!");
    }
    memset(new_pin,0,6);
    memset(new_pin_2,0,6);
    hardware_wallet.GetModeState(buf);
    Serial.print("[Re-init SE] Mode:");Serial.print(buf[0],HEX);Serial.print("==>");Serial.println(SE_Mode[buf[0]]);
    Serial.print("[Re-init SE] State:");Serial.println(buf[1],HEX);

    Serial.println("[Re-init SE] InitWalletFromMnemonic");
    display_initwalletFrommemonic();
    ret = InitWalletFromMnemonic(Memonic);

    Serial.println("[Re-init SE] hdw_query_wallet_info");
    retry = 3;
    while(retry){
      ret = hdw_query_wallet_info();
      if (ret != 0)
      {
          Serial.print("[Re-init SE] hdw_query_wallet_info Failed! Stop the reinit procedure");
          if(retry==1){
              secureelement_error();
          }
      }
      else{
          Serial.print("[Re-init SE] hdw_query_wallet_info Success!");
          break;
      }
      retry --;
    }

    Serial.println("[Re-init SE] Creating Account");
    ret = hdw_create_account(0x00);
    if (ret != 0)
    {
        Serial.print("[Re-init SE] hdw_create_account Failed! Stop the reinit procedure");
        secureelement_error();
    }

    Serial.println("[Re-init SE] Generate Account Public Key");
    ret = hdw_generate_next_trx_addr(0x00);
    if (ret != 0)
    {
        Serial.print("[Re-init SE] hdw_generate_next_trx_addr Failed! Stop the reinit procedure");
        secureelement_error();
    }
  
    Serial.println("[Re-init SE] Query Account Key info");
    ret = hdw_query_account_key_info(0x00);
    if (ret != 0)
    {
        Serial.print("[Re-init SE] hdw_query_account_key_info Failed! Stop the reinit procedure");
        secureelement_error();
    }
    digitalWrite(A0,LOW);
}
void quary_erc20s(){
    char group_name[64];
    char data_item_name[64];
    nvdm_status_t status;
    std::vector<String> ERC20_names;
    erc20_struct hitconToken;
    memcpy(hitconToken.Contract_Address,hitconToken_address,20);
    memcpy(hitconToken.Name,hitconToken_Name,4);
    addERC20(hitconToken);


    status = nvdm_query_begin();
    if (status != NVDM_STATUS_OK) {
        Serial.println("[quary_erc20s] NVDM_STATUS_ERROR");
    }
    while(1){
        status = nvdm_query_next_group_name(group_name);
        if (status != NVDM_STATUS_OK) {
            status = nvdm_query_end();
            return;
        }
        //Serial.println(group_name);
        if(strcmp ("ERC20",group_name)==0){
            Serial.println("[quary_erc20s] ERC20 Found");
            break;
        }
    }

    
    while(1){
        status = nvdm_query_next_data_item_name(data_item_name);
        if (status != NVDM_STATUS_OK) {
            status = nvdm_query_end();
            break;
        }
        Serial.print("[quary_erc20s] New ERC20 Get:");Serial.println(data_item_name);
        ERC20_names.push_back(String(data_item_name));
    }

    for (int i = 0; i < ERC20_names.size(); ++i)
    {
        erc20_struct new_erc20;
        uint8_t buffer[100]= {0};
        uint32_t size = 100;
        LFlash.read("ERC20", ERC20_names[i].c_str(), (uint8_t*)&buffer, &size);
        
        memcpy(new_erc20.Contract_Address,buffer,20);
        memcpy(new_erc20.Name,ERC20_names[i].c_str(),4);
        if (status != NVDM_STATUS_OK) {
            Serial.print("[quary_erc20s] Read Error:");Serial.println(status);
        }
        else{
            erc20s.push_back(new_erc20);
        }
    }
    for (int i = 0; i < erc20s.size(); ++i)
    {
        Serial.print("[quary_erc20s] ERC20:");Serial.println(i);
        printERC20(erc20s[i]);  
    }

}
void addERC20(erc20_struct erc20){
    nvdm_write_data_item("ERC20",(const char*)erc20.Name, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&erc20.Contract_Address, 20);
}
void printERC20(erc20_struct erc20){
    Serial.print("\tContract Address:");Serial.println(ArraytoString(erc20.Contract_Address,20));
    Serial.print("\tName:");Serial.println((char*)erc20.Name);
}

uint8_t start_transaction(uint8_t* outputdata,transaction_struct trx_data){
  
  Transaction trx;
  vector<uint8_t> rawTransaction = trx.GenerateSignature(trx_data);

  int ret = 0;
  uint8_t inputID = 0;
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t key_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
  uint8_t SECPO[BCDC_SECPO_LEN];
  uint8_t rcv_buf[1024] = { 0 };

  uint8_t infoID = 3;    //HDW_ACCINFO_BALNC 3
  uint8_t balance[HDW_ACC_BAL_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, };
  ret = hardware_wallet.hdw_set_acc_info(infoID, acc_id, balance, rcv_buf);


  uint8_t outAddr[OUTADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00 };
  ret = hardware_wallet.hdw_prep_trx_sign(inputID, KeyChain, acc_id, key_id, balance, &rawTransaction[0], rcv_buf);
  if (ret != 0) {
    outputdata[0] = 0xF2;
    hardware_wallet.hdw_trx_sign(0, rcv_buf);
    hardware_wallet.hdw_trx_sign_finish(rcv_buf);
    return 1;
  }
  ret = hardware_wallet.hdw_trx_sign_begin(balance, outAddr, rcv_buf);
  if (ret != 0) {
    outputdata[0] = 0xF2;
    hardware_wallet.hdw_trx_sign(0, rcv_buf);
    hardware_wallet.hdw_trx_sign_finish(rcv_buf);
    return 1;
  }

  ret = hardware_wallet.GetSECPO(SECPO);
  if (ret != 0) {
    outputdata[0] = 0xF2;
    return 1;
  }
  Serial.print("[Transaction] SECPO[0]");Serial.println(SECPO[0],BIN);
  if (SECPO[0] & SECPO_MASK_PIN) {
        uint8_t pinCode[6];
        int i;
        for (i = 0; i < 3; ++i) {
            pincode_page(pinCode,i);
            ret = hardware_wallet.hdw_trx_verify(pinCode, SECPO_MASK_PIN);
            if (ret == 0){
                pincode_success();
                break;
            }
            else{
                if (i == 2)
                {
                    pincode_error();
                }
            }
        memset(pinCode,0,6);
        }
  }

  ret = hardware_wallet.hdw_trx_sign(0, rcv_buf);
  if (ret != 0) {
    outputdata[0] = 0xF2;
    hardware_wallet.hdw_trx_sign_finish(rcv_buf);
    return 1;
  }
  Serial.println("[Transaction] SIGNATURE data:");
  hardware_wallet.dumpData("SIGNATURE",rcv_buf,65);

  uint8_t signature[65] = {0};
  memcpy(signature,rcv_buf,65);

  vector<uint8_t> param = trx.RlpEncodeForRawTransaction(trx_data,signature,signature[64]);
  Serial.print("[Transaction] Txn out:");print_vector(param);Serial.println();
  hardware_wallet.hdw_trx_sign_finish(rcv_buf);
  if (param.size()>256)
  {
      Serial.println("[Transaction] Txn length too large");
      outputdata[0] = 0xF3;
      return 1;
  }
  else{
    memcpy(outputdata, param.data(),param.size());
    return param.size();
  }
}
uint8_t start_transaction_cmdline(uint8_t* outputdata,uint32_t account,uint32_t key,transaction_struct trx_data){
  
  Transaction trx;
  vector<uint8_t> rawTransaction = trx.GenerateSignature(trx_data);

  int ret = 0;
  uint8_t inputID = 0;
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };

    for (int i = 0; i < 4; ++i)
  {
      acc_id[i] = account >> (i*8);
  }

  uint8_t key_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
    for (int i = 0; i < 4; ++i)
  {
      key_id[i] = key >> (i*8);
  }

  uint8_t SECPO[BCDC_SECPO_LEN];
  uint8_t rcv_buf[1024] = { 0 };

  uint8_t infoID = 3;    //HDW_ACCINFO_BALNC 3
  uint8_t balance[HDW_ACC_BAL_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, };
  ret = hardware_wallet.hdw_set_acc_info(infoID, acc_id, balance, rcv_buf);


  uint8_t outAddr[OUTADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00 };
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
  Serial.print("[Transaction] SECPO[0]");Serial.println(SECPO[0],BIN);
  if (SECPO[0] & SECPO_MASK_PIN) {
        int i;
        for (i = 0; i < 3; ++i) {
            uint8_t pinCode[6];
            uint8_t pinCode_count = 0;
            if (i>0)
            {
                Serial.print("Retry:");Serial.println(i);
            }
            Serial.print("[Transaction] Please enter Pin Code:");
            while(1){
                char c = Serial.read();
                if (c == '\n' || c == '\r' && pinCode_count == 6)
                {
                    Serial.println();
                    break;
                }
                if (c <=0x39 && c >=0x30 && pinCode_count < 6) //numbers
                {
                    Serial.write(c);
                    pinCode[pinCode_count] = c;
                    pinCode_count ++;
                }
                if (c == '\b' && pinCode_count>0) //backspace
                {
                    pinCode_count --;
                    Serial.write(c);
                }
            }
            ret = hardware_wallet.hdw_trx_verify(pinCode, SECPO_MASK_PIN);
            if (ret == 0){
                Serial.println("[Transaction] Pin Code Pass");
                break;
            }
            else{
                if (i == 2)
                {
                    Serial.println("[Transaction] Pin Code Error, canceled transaction");
                }
            }
        }
  }

  ret = hardware_wallet.hdw_trx_sign(0, rcv_buf);
  if (ret != 0) {
    return ret;
  }
  Serial.println("[Transaction] SIGNATURE data:");
  hardware_wallet.dumpData("SIGNATURE",rcv_buf,65);

  uint8_t signature[65] = {0};
  memcpy(signature,rcv_buf,65);

  vector<uint8_t> param = trx.RlpEncodeForRawTransaction(trx_data,signature,signature[64]);
  hardware_wallet.hdw_trx_sign_finish(rcv_buf);
  if (param.size()>256)
  {
      Serial.println("[Transaction] Txn length too large");
      outputdata[0] = 0xF3;
      return 1;
  }
  else{
    memcpy(outputdata, param.data(),param.size());
    return param.size();
  }
}
uint64_t read_balance(){
  uint64_t balance = 0;
  uint32_t size;
  LFlash.read("Wallet", "BLANCE", (uint8_t *)&balance, &size);
  return balance;
}


