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
  Serial.println("Hello!");
  delay(10);
  Serial.println("Jump");
  hardware_wallet.loader_cmdhdlr_CosJump();
  Serial.println("Jump Success");
  delay(1000);

  uint8_t ret = BindLoginTest();
  Serial.println(ret);


  ret = hdw_query_wallet_info();
  Serial.println("hdw_create_account_test");
  ret = hdw_create_account_test();
  Serial.println(ret);

}

void loop(void) {
  delay(1000000);
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
static int hdw_create_account_test(void)
{
	int ret = 0;
	uint8_t rcv_buf[64] = { 0 };
	uint8_t acc_name[HDW_ACCNAME_LEN] = { 0 };
	uint8_t acc_purpose[HDW_ACC_PURPOSE_LEN] = { 0x2c, 0x00, 0x00, 0x80 };      //44
	uint8_t acc_cointype[HDW_ACC_COINTYPE_LEN] = { 0x01, 0x00, 0x00, 0x80 };    //bitcoin testnet
	//uint8_t acc_cointype[HDW_ACC_COINTYPE_LEN] = { 0x3c, 0x00, 0x00, 0x80 };    //ETH
	uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x02, 0x00, 0x00, 0x00 };
	char *tmp = "ikv_test";

	memcpy(acc_name, (uint8_t *)tmp, strlen(tmp));
	ret = hardware_wallet.hdw_create_account(acc_name, acc_purpose, acc_cointype, acc_id, rcv_buf);

	acc_cointype[0] = 0x3c; //ETH
	acc_id[0] = 0x03;
	ret = hardware_wallet.hdw_create_account(acc_name, acc_purpose, acc_cointype, acc_id, rcv_buf);

	return ret;
}
static int hdw_query_account_info(void)
{
	int ret = 0;
	uint8_t acc_id[HDW_ACC_ID_LEN] = { 0x00, 0x00, 0x00, 0x00 };
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
