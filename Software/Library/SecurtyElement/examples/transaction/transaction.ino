#include <SPI.h>
#include <wallet.h>
#include <LWiFi.h>
#include "LFlash.h"
#include <Transaction.h>
#include <CryptoHelper.h>
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
  memcpy(hardware_wallet.host_cred,mac,6);

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
  Serial.println("Hello!");
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

  uint8_t ret = BindLoginTest();
  Serial.println(ret);

  hdw_query_account_key_info(0x02);


  ret = CreateETH_Trx();
  Serial.println(ret);



}


void loop(void) {
  
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
  memcpy(publickey,rcv_buf,HDW_KPUB_LEN);
  
  vector<uint8_t> address = cyp.generateAddress(publickey);

  Serial.println("Address: ");

   for (int i = 0; i < address.size(); ++i)
  {
    if (address[i]<=0xF)
    {
     Serial.print("0");
    }
    Serial.print(address[i],HEX);
  }
  Serial.println();



  return ret;
}

static int BindLoginTest(void)
{
  int ret = 0;
  uint8_t cred_buf[64] = { 0 };
  uint8_t chlng[BIND_CHLNG_LEN] = { 0 };
  uint8_t rcv_buf[1024] = { 0 };

  Serial.println("reading cred");
  uint32_t size =  64;
  LFlash.read("SE","CRED",(uint8_t *)&cred_buf,&size);
  hardware_wallet.dumpData("credential", cred_buf,size);

  //hardware_wallet.ReadCred(cred_buf);
  hardware_wallet.dumpData("credential", cred_buf, 1 + BIND_OTP_LEN);

  ret = hardware_wallet.BindLoginChallenge(cred_buf[0], chlng);
  if (ret != 0) {
    goto __func_end;
  }
  hardware_wallet.dumpData("BindLoginChallenge", chlng, BIND_CHLNG_LEN);

  ret = hardware_wallet.BindLogin(cred_buf[0], hardware_wallet.host_cred, cred_buf + 1, chlng, rcv_buf);
  if (ret != 0) {
    goto __func_end;
  }
  Serial.println("Login Success");


  memcpy(cred_buf+1+BIND_OTP_LEN, chlng, BIND_CHLNG_LEN);
  hardware_wallet.dumpData("cred_buf", cred_buf, 1+BIND_OTP_LEN+BIND_CHLNG_LEN); 
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

static int CreateETH_Trx(void)
{
  int ret = 0;
  uint8_t inputID = 0;
  uint8_t KeyChain = 0;
  uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x02, 0x00, 0x00, 0x00 };
  uint8_t key_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };

  uint8_t rcv_buf[1024] = { 0 };

  uint8_t infoID = 3;    //HDW_ACCINFO_BALNC 3
  uint8_t balance[HDW_ACC_BAL_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, };
  ret = hardware_wallet.hdw_set_acc_info(infoID, acc_id, balance, rcv_buf);
/*
  //eth_tx.nonce = "0x0b";
  eth_tx.gasPrice = "0x04a817c800";
  eth_tx.gasLimit = "0x0249f0";
  //eth_tx.to = "0xCE26faeeC22f1D13f4C92072E32f2fb3c8134297";
  //eth_tx.to = "0xc1f8C2646F51B376C6048a06dCBc8A8FE9554432";
  eth_tx.to = "0x0243cD29C2cc7299d60A53B56048421e5fE44863";
  //eth_tx.value="0x0de0b6b3a7640000";   //1
  eth_tx.value = "0x0214e8348c4f0000";   //0.15
  eth_tx.data = "0xc0de";

  str = rlp.encodeTx(eth_tx, false);

  keccak_init(&context);
  keccak_update(&context, (const unsigned char*)str.c_str(), str.size());
  keccak_final(&context, (unsigned char*)trxHash);
*/
//Transaction::GenerateSignature(uint8_t * nonceVal,uint8_t * gasPriceVal,uint8_t * gasLimitVal,uint8_t* toStr,uint8_t* valueStr,uint8_t* dataStr,uint32_t chainId);
  std::vector<uint8_t> trxHash =  trx.GenerateSignature((uint8_t*)"0x00",(uint8_t*)"0x04a817c800",(uint8_t*)"0x0249f0",(uint8_t*)"0x0243cD29C2cc7299d60A53B56048421e5fE44863",(uint8_t*)"0x0214e8348c4f0000",(uint8_t*)"0x00",3);


  for (int i = 0; i < trxHash.size(); ++i)
  {
    Serial.print(trxHash[i],HEX);
  }
  Serial.println();
  uint8_t outAddr[OUTADDR_LEN] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                   0x00, 0x00 };
  ret = hardware_wallet.hdw_prep_trx_sign(inputID, KeyChain, acc_id, key_id, balance, &trxHash[0], rcv_buf);
  if (ret != 0) {
    return ret;
  }

  ret = hardware_wallet.hdw_trx_sign_begin(balance, outAddr, rcv_buf);
  if (ret != 0) {
    return ret;
  }

  ret = hardware_wallet.hdw_trx_sign(0, rcv_buf);
  if (ret != 0) {
    return ret;
  }
  int recoveryId[1] = {0};
  uint8_t signature[64];
  memset(signature, 0, 64);
  memcpy(signature,rcv_buf,64);
  vector<uint8_t> param = trx.RlpEncodeForRawTransaction((uint8_t*)"0x00",(uint8_t*)"0x04a817c800",(uint8_t*)"0x0249f0",(uint8_t*)"0x0243cD29C2cc7299d60A53B56048421e5fE44863",(uint8_t*)"0x0214e8348c4f0000",(uint8_t*)"0x00",3,signature,0);

/*
  eth_tx.r = byte2HexString(rcv_buf, BCDC_TRX_SIGLEN / 2);
  eth_tx.s = byte2HexString(rcv_buf + (BCDC_TRX_SIGLEN / 2), BCDC_TRX_SIGLEN / 2);
  rcv_buf[BCDC_TRX_SIGLEN - 1] += 0x1b;
  eth_tx.v = byte2HexString(rcv_buf + BCDC_TRX_SIGLEN - 1, 1);

  std::cout << "r " << eth_tx.r << std::endl;
  std::cout << "s " << eth_tx.s << std::endl;
  std::cout << "v " << eth_tx.v << std::endl;

  //https://rinkeby.etherscan.io/pushTx
  str = rlp.encodeTx(eth_tx, true);
  str = prefix + byte2HexString((uint8_t *)str.c_str(), str.size());
*/
  for (int i = 0; i < param.size(); ++i)
  {
    if (param[i]<=0xF)
    {
     Serial.print("0");
    }
    Serial.print(param[i],HEX);
  }
  Serial.println();

  hardware_wallet.hdw_trx_sign_finish(rcv_buf);

  return ret;
}
