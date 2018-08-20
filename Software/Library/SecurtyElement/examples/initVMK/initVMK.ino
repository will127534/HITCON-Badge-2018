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
  hardware_wallet.init(SE_SS);
  
      if (LFlash.begin() != LFLASH_OK)
    {
        Serial.println("Flash init failed.");
        return;
    }
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

  Serial.println("Hello!");

  uint8_t buf[32] = {0}; 
  
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
//  uint8_t ret = InitVMK();
//  Serial.println(ret);

  uint8_t ret = InitDevInfo();
  Serial.println(ret);
}


void loop(void) {
  
}


static int InitVMK(void)
{
	int ret = 0;
	int retry = 0;
	uint8_t chlng[AUTHUTIL_CHLNG_LEN];
	uint8_t *curVMK;

	curVMK = vmk_default;

	CHANGE_VMK_RETRY:
	Serial.println("init VMK change chlng");
		ret = hardware_wallet.InitVMK_CHLNG(chlng);
		if (ret != 0)
			return ret;
	Serial.println("init VMK change");
		ret = hardware_wallet.InitChangeVMK(chlng, curVMK, vmk_new);
		if (ret != 0) {
			printf("Cannot change VMK by default VMK! Try to use another VMK\n");
			//The new VMK will not be clear after backing to init
			//So vendor must save their VMK carefully.
			//InitChangeVMK(chlng, current VMK, new VMK);
			if (retry == 0) {
				retry = 1;
				curVMK = vmk_new;
				goto CHANGE_VMK_RETRY;
			}
		}

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
	//	return ret;
	//}

	ret = hardware_wallet.InitDataConfirm();
	if (ret != 0) {
			return ret;
	}

	return ret;
}