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
  memcpy(hardware_wallet.host_cred,mac,6);
  hardware_wallet.dumpData("host_cred", hardware_wallet.host_cred, 32);

  if (LFlash.begin() != LFLASH_OK)
  {
    Serial.println("Flash init failed.");
    return;
  }


  uint32_t size =  64;
  uint8_t Cred_buf[64] = {0}; 
  LFlash.read("SE","CRED",(uint8_t *)&Cred_buf,&size);

  hardware_wallet.WriteCred(Cred_buf);
  hardware_wallet.init(SE_SS);

  waitforcmd();

  uint8_t buf[2] = {0}; 
  hardware_wallet.GetModeState(buf);
  Serial.print("Mode:");Serial.println(buf[0],HEX);
  Serial.print("State:");Serial.println(buf[1],HEX);

  if (buf[1] == 0x10) // bootloader state
  {
    delay(10);
    Serial.println("Jump");
    hardware_wallet.loader_cmdhdlr_CosJump();
    Serial.println("Jump Success");
    delay(1000);
  }
  waitforcmd();

  uint8_t ret;
  
  Serial.println("Init Device InFo");
  ret = InitDevInfo();
  Serial.println(ret);
  waitforcmd();

  hardware_wallet.GetModeState(buf);
  Serial.print("Mode:");Serial.println(buf[0],HEX);
  Serial.print("State:");Serial.println(buf[1],HEX);
  waitforcmd();

  Serial.println("Bind Reg");
  ret = BindReg();
  Serial.println(ret);
  waitforcmd();

  hardware_wallet.GetModeState(buf);
  Serial.print("Mode:");Serial.println(buf[0],HEX);
  Serial.print("State:");Serial.println(buf[1],HEX);
  waitforcmd();

  Serial.println("Bind Login");
  ret = BindLogin();
  Serial.println(ret);
  waitforcmd();

  hardware_wallet.GetModeState(buf);
  Serial.print("Mode:");Serial.println(buf[0],HEX);
  Serial.print("State:");Serial.println(buf[1],HEX);
  waitforcmd();

  Serial.println("InitWalletFromMnemonic");
  ret = InitWalletFromMnemonic();
  Serial.println(ret);
  waitforcmd();

  Serial.println("hdw_query_wallet_info");
  ret = hdw_query_wallet_info();
  Serial.println(ret);
  waitforcmd();

  Serial.println("Creating Account");
  ret = hdw_create_account_test(0x00);
  Serial.println(ret);
  waitforcmd();

  Serial.println("Generate Account Key");
  ret = hdw_generate_next_trx_addr(0x00);
  Serial.println(ret);
  waitforcmd();

  Serial.println("Query Account Key info");
  ret = hdw_query_account_key_info(0x00);
  Serial.println(ret);
}
void loop(void) {
  delay(1000000);
}

void waitforcmd(){
  Serial.println("Wait for User input...");
  while(!Serial.available()){
    delay(100);
  }
  while(Serial.available()){
    char temp = Serial.read();
  }
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
    firstHost = firstHost ? 0 : 1;
    ret = hardware_wallet.BindRegInit(firstHost, hardware_wallet.host_cred, hardware_wallet.host_desc, rcv_buf);
  }
  if (ret != 0){
    Serial.println("BindRegInit Error!");
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
    //To-Do : save it by Windows.Security.Credentials or CryptProtectData ?
    //dumpdata("BindRegFinish", rcv_buf, 2);         //host_id, bind_state
    //need to save OTP key for bind login
    /*
    fwrite(BIND_CRED_FILE_HEADER, 1, strlen(BIND_CRED_FILE_HEADER), fp);
    fwrite(rcv_buf, 1, 1, fp);                       //HostID
    fwrite(rcv_buf + 2, 1, BIND_OTP_LEN, fp);        //skip confirm state, RegOtp for bind login
    fwrite(chlng, 1, BIND_CHLNG_LEN, fp);            //space for saving loging challenge
    fclose(fp);
    */
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

static int BindLogin(void)
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t chlng[BIND_CHLNG_LEN] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };

  Serial.println("reading cred");
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
  Serial.println("Login Success");


  memcpy(cred_buf+1+BIND_OTP_LEN, chlng, BIND_CHLNG_LEN);
  //hardware_wallet.dumpData("cred_buf", cred_buf, 1+BIND_OTP_LEN+BIND_CHLNG_LEN); 
  LFlash.write("SE","CRED",LFLASH_RAW_DATA,(const uint8_t *)&cred_buf,1+BIND_OTP_LEN+BIND_CHLNG_LEN);  
  hardware_wallet.WriteCred(cred_buf);




  //store login challenge

  //fseek(fp, ftell(fp), SEEK_SET);
  //fwrite(chlng, 1, BIND_CHLNG_LEN, fp);

  ret = hardware_wallet.GetModeState(rcv_buf);
  
  hardware_wallet.dumpData("GetModeState", rcv_buf, 32);
  if (ret != 0) {
    goto __func_end;
  }

  //perso mode, set the perso data and then enter normal mode
  if (rcv_buf[0] == 1) {
    ret = hardware_wallet.perso_set_data(hardware_wallet.host_cred, cred_buf + 1, chlng, rcv_buf);
    if (ret != 0)
      goto __func_end;
    ret = hardware_wallet.perso_confirm(rcv_buf);
    if (ret != 0)
      goto __func_end;
  }

  __func_end:
  return ret;
}

static int InitDevInfo(void)
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
  /*
  ret = GetInitDataHash(BCDC_IDATA_UPINHASH, 0, HASH);
  if (ret != 0) {
    return ret;
  }
  dumpdata("BCDC_IDATA_UPINHASH HASH", HASH, PINHASH_LEN);

  ret = GetInitDataHash(BCDC_IDATA_PUK, 0, HASH);
  if (ret != 0) {
    return ret;
  }
  dumpdata("BCDC_IDATA_PUK HASH", HASH, PINHASH_LEN);

  ret = GetInitDataHash(BCDC_IDATA_CARDID, 0, HASH);
  if (ret != 0) {
    return ret;
  }
  dumpdata("BCDC_IDATA_CARDID HASH", HASH, PINHASH_LEN);
  */
  //change vendor master key which is necessary for backing to init state
  //ret = InitVMK();
  //if (ret != 0) {
  //  return ret;
  //}

  ret = hardware_wallet.InitDataConfirm();
  if (ret != 0) {
      return ret;
  }

  return ret;
}


static int InitWalletFromMnemonic(void)
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };

  char *hdw_name = "Testing Wallet";  //HDW_NAME_LEN
  //char *passphrase = "RaymondLiou";
  char *mnemonic   = "uphold daring answer grief noble morning slim talent faint foster melt company young track popular chapter title oven absurd tuition shop luxury settle always";
  //seed0 : mnemonic /wo  passphrase
  Serial.println("mnemonic /wo  passphrase");
  ret = hardware_wallet.hdw_init_wallet_words((uint8_t *)hdw_name, NULL, 0, mnemonic, strlen(mnemonic), rcv_buf);

  return ret;
}
static int hdw_query_wallet_info(void)
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

static int hdw_create_account_test(uint8_t id)
{
  int ret = 0;
  uint8_t rcv_buf[64] = { 0 };
  uint8_t acc_name[HDW_ACCNAME_LEN] = { 0 };
  uint8_t acc_purpose[HDW_ACC_PURPOSE_LEN] = { 0x2c, 0x00, 0x00, 0x80 };      //44
  uint8_t acc_cointype[HDW_ACC_COINTYPE_LEN] = { 0x01, 0x00, 0x00, 0x80 };    //bitcoin testnet
  //uint8_t acc_cointype[HDW_ACC_COINTYPE_LEN] = { 0x3c, 0x00, 0x00, 0x80 };    //ETH
  acc_cointype[0] = 0x3c; //ETH
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x02, 0x00, 0x00, 0x00 };
  char *tmp = "eth_test";
  acc_id[0] = id;
  memcpy(acc_name, (uint8_t *)tmp, strlen(tmp));
  ret = hardware_wallet.hdw_create_account(acc_name, acc_purpose, acc_cointype, acc_id, rcv_buf);
  return ret;
}
static int hdw_query_account_info(uint8_t id)
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

