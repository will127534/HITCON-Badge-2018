#include "wallet.h"
#include "Arduino.h"
#include "hal_sha.h"
#include "hal_aes.h"
#include "hmac256.h"
#define OUT 
#define IN
#include "LFlash.h"

uint8_t vmk_default[VMK_LEN] = { 0 };
uint8_t vmk_new[VMK_LEN] = { 0 };
uint8_t PIN_DEF[6] = { 0x30, 0x31, 0x32, 0x33, 0x32, 0x31 };
uint8_t PIN_UNLOCK[6] = { 0x37, 0x35, 0x35, 0x36, 0x36, 0x38 };

wallet::wallet() : Securty_Element() {
	
}

int wallet::WriteCred(uint8_t IN *Cred_buf)
{      
	memcpy(_Cred_buf,Cred_buf, 1 + BIND_OTP_LEN + BIND_CHLNG_LEN);
	return 0;
}

int wallet::ReadCred(uint8_t OUT *Cred_buf)
{
	memcpy(Cred_buf,_Cred_buf, 1 + BIND_OTP_LEN + BIND_CHLNG_LEN);
	return 0;
}

/*
 * [Description  ] Get mode state
 * [Response data] [MODE (1 byte)] [STATE (1 byte)] 
 */
int wallet::GetModeState(uint8_t OUT *respdata)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x02, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_GET_MODE_STATE;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, respdata, 2) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Set Init data
 * [In Data      ] [Init Data ID(1 byte)] [PreHost ID(1 byte)] [INITDATA (variable length)] [INITDATA LEN]
 * [Response data]
 */
int wallet::SetInitData(uint8_t IN ID, uint8_t IN preHostID, uint8_t IN *initData, uint8_t IN rawlen)
{
	hal_sha256_context_t context = {0};
	uint8_t cmd_buf[CMD_LEN + PINHASH_LEN * 2] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };

	uint16_t data_len = 0;
	uint8_t rspdata[64];

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_INIT_SET_DATA;
	cmd_buf[2] = ID;
	cmd_buf[3] = preHostID;

	if ((ID == BCDC_IDATA_CARDID) && (rawlen == CARD_ID_LEN)) {
		data_len = CARD_ID_LEN;
		memcpy(cmd_buf + CMD_LEN, initData, data_len);
	} else {
		data_len = PINHASH_LEN;
		//raw pin to hash
		
		hal_sha256_init(&context);
		hal_sha256_append(&context, initData, rawlen);
		hal_sha256_end(&context, cmd_buf + CMD_LEN);

		/*
		sha256_init(&ctx);
		sha256_update(&ctx, initData, rawlen);
		sha256_final(&ctx, cmd_buf + CMD_LEN);
		*/
	}
	cmd_buf[4] = (data_len + PINHASH_LEN) & 0xFF;
	cmd_buf[5] = ((data_len + PINHASH_LEN) >> 8) & 0xFF;

	hal_sha256_init(&context);
	hal_sha256_append(&context, cmd_buf + CMD_LEN, data_len);
	hal_sha256_end(&context, cmd_buf + CMD_LEN + data_len);

	/*
	sha256_init(&ctx);
	sha256_update(&ctx, cmd_buf + CMD_LEN, data_len);
	sha256_final(&ctx, cmd_buf + CMD_LEN + data_len);
	//dumpdata("HASH", cmd_buf + CMD_LEN + data_len, PINHASH_LEN);
	*/

	memcpy(cmd_buf + CMD_LEN + data_len + PINHASH_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + data_len + PINHASH_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

/*
* [Description  ] Get Init data Hash
* [In Data      ] [Init Data ID(1 byte)] [PreHost ID(1 byte)]
* [Response data] [Hash of init data]
*/
int wallet::GetInitDataHash(uint8_t IN ID, uint8_t IN preHostID, uint8_t OUT *Hash)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_INIT_GET_DATA_HASH;
	cmd_buf[2] = ID;
	cmd_buf[3] = preHostID;

	cmd_le_dly_rty[0] = PINHASH_LEN & 0xFF;
	cmd_le_dly_rty[1] = (PINHASH_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, Hash, PINHASH_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

/*
 * [Description  ] Init VMK(vnedor master key) Challenge
 * [In Data      ]
 * [Response data] [CHLNG (16 bytes)] 
 */
int wallet::InitVMK_CHLNG(uint8_t OUT *chlng)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_INIT_VMK_CHLNG;

	cmd_le_dly_rty[0] = AUTHUTIL_CHLNG_LEN & 0xFF;
	cmd_le_dly_rty[1] = (AUTHUTIL_CHLNG_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);
	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, chlng, AUTHUTIL_CHLNG_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

/*
 * [Description  ] Init VMK(vnedor master key) Challenge
 * [In Data      ] [VMK Chllange (16 bytes)] [current VMK(32 bytes)] [NEW VMK (32 bytes)]
 * [Response data] 
 */
int wallet::InitChangeVMK(uint8_t IN *vmkChlng,uint8_t IN *curVMK, uint8_t IN *newVMK)
{
	hal_sha256_context_t context = {0};
	uint8_t cmd_buf[CMD_LEN + AUTHUTIL_RESP_LEN + VMK_LEN + HAL_SHA256_DIGEST_SIZE + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	uint8_t evmk[VMK_LEN] = { 0 };
	uint8_t vmkResp[AUTHUTIL_RESP_LEN] = { 0 };
	uint8_t rspdata[64];

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_INIT_CHANGE_VMK;
	cmd_buf[4] = (AUTHUTIL_RESP_LEN + VMK_LEN + HAL_SHA256_DIGEST_SIZE) & 0xFF;
	cmd_buf[5] = ((AUTHUTIL_RESP_LEN + VMK_LEN + HAL_SHA256_DIGEST_SIZE) >> 8) & 0xFF;

	//RMD temp
	//ikvce_aes_key_initialize(vmk_default, 32, MOP_ECB, ACT_DEC, NULL);
	//ikvce_aes_crypt(curVMK, VMK_LEN, curVMK, VMK_LEN / IKVCE_BLOCK_SIZE_AES);

	//vmkresp = enc(challenge, vmk)


	hal_aes_buffer_t plain_text = {
	    .buffer = vmkChlng,
	    .length = AUTHUTIL_RESP_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = curVMK,
	    .length = 32
	};
	uint8_t encrypted_buffer[32] = {0};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = encrypted_buffer,
	    .length = 32
	};

	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
	//dumpData("vmkChlng",vmkChlng,AUTHUTIL_RESP_LEN);
	//dumpData("curVMK",curVMK,32);
	//dumpData("encrypted_buffer",encrypted_buffer,32);

	hal_aes_buffer_t plain_new_emk = {
	    .buffer = newVMK,
	    .length = VMK_LEN
	};
	uint8_t new_emk[32+16] = {0};
	hal_aes_buffer_t encrypted_new_emk = {
	    .buffer = new_emk,
	    .length = 32+16
	};


	hal_aes_ecb_encrypt(&encrypted_new_emk, &plain_new_emk, &key);
	//dumpData("newVMK",newVMK,VMK_LEN);
	//dumpData("curVMK",curVMK,32);
	//dumpData("encrypted_buffer",new_emk,32+16);


	//ikvce_aes_key_initialize(curVMK, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(vmkResp, AUTHUTIL_RESP_LEN, vmkChlng, AUTHUTIL_RESP_LEN / IKVCE_BLOCK_SIZE_AES);
    //encrypt new VMK by current VMK
	//ikvce_aes_crypt(evmk, VMK_LEN, newVMK, VMK_LEN / IKVCE_BLOCK_SIZE_AES);

	memcpy(cmd_buf + CMD_LEN, encrypted_buffer, AUTHUTIL_RESP_LEN);
	memcpy(cmd_buf + CMD_LEN + AUTHUTIL_RESP_LEN, new_emk, VMK_LEN);

	hal_sha256_init(&context);
	hal_sha256_append(&context, cmd_buf + CMD_LEN + AUTHUTIL_RESP_LEN, VMK_LEN);
	hal_sha256_end(&context, cmd_buf + CMD_LEN + AUTHUTIL_RESP_LEN + VMK_LEN);
	/*
	sha256_init(&ctx);
	sha256_update(&ctx, cmd_buf + CMD_LEN + AUTHUTIL_RESP_LEN, VMK_LEN);
	sha256_final(&ctx, cmd_buf + CMD_LEN + AUTHUTIL_RESP_LEN + VMK_LEN);
	*/

	memcpy(cmd_buf + CMD_LEN + AUTHUTIL_RESP_LEN + VMK_LEN + HAL_SHA256_DIGEST_SIZE, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + AUTHUTIL_RESP_LEN + VMK_LEN + HAL_SHA256_DIGEST_SIZE + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;

}


/*
 * [Description  ] Init Back Init
 * [In Data      ] [VMK Challenge (16 bytes)] 
 * [Response data]
 */
int wallet::InitBackInit(uint8_t IN *vmkChlng, uint8_t IN *curVMK)
{
	int i;
	uint8_t rspdata[64];
	uint8_t cmd_buf[CMD_LEN + AUTHUTIL_RESP_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x20, 0x00,
											   0x01, 0x00, };
	uint8_t vmkResp[AUTHUTIL_RESP_LEN+16] = { 0 };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_INIT_BACK_INIT;
	cmd_buf[4] = AUTHUTIL_RESP_LEN & 0xFF;
	cmd_buf[5] = (AUTHUTIL_RESP_LEN >> 8) & 0xFF;

	//vmkresp = enc(challenge, vmk)
	hal_aes_buffer_t plain_text = {
	    .buffer = vmkChlng,
	    .length = AUTHUTIL_RESP_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = curVMK,
	    .length = 32
	};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = vmkResp,
	    .length = AUTHUTIL_RESP_LEN+16
	};


	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
	//ikvce_aes_key_initialize(curVMK, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(vmkResp, AUTHUTIL_RESP_LEN, vmkChlng, AUTHUTIL_RESP_LEN / IKVCE_BLOCK_SIZE_AES);
	memcpy(cmd_buf + CMD_LEN, vmkResp, AUTHUTIL_RESP_LEN);
	memcpy(cmd_buf + CMD_LEN + AUTHUTIL_RESP_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + AUTHUTIL_RESP_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
	}

	return 0;
}

/*
 * [Description  ] Init data Confirm
 * [In Data      ] 
 * [Response data]
 */
int wallet::InitDataConfirm(void)
{
	uint8_t rspdata[64] = { 0 };
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_INIT_CONFIRM;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;

}

/*
 * [Description  ] Set Card Name
 * [In Data      ] [Name (32 bytes)]
 * [Response data]
 */
int wallet::SetCardName(uint8_t IN *Name)
{
	uint8_t rspdata[64] = { 0 };
	uint8_t cmd_buf[CMD_LEN + BCDC_CARDNAME_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_SET_CARD_NAME;
	cmd_buf[4] = BCDC_CARDNAME_LEN & 0xFF;
	cmd_buf[5] = (BCDC_CARDNAME_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, Name, BCDC_CARDNAME_LEN);
	memcpy(cmd_buf + CMD_LEN + BCDC_CARDNAME_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + BCDC_CARDNAME_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;

}

/*
 * [Description  ] Get Card Name
 * [In Data      ]
 * [Response data] [Name (32 bytes)]
 */
int wallet::GetCardName(uint8_t OUT *rspdata)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_GET_CARD_NAME;

	cmd_le_dly_rty[0] = BCDC_CARDNAME_LEN & 0xFF;
	cmd_le_dly_rty[1] = (BCDC_CARDNAME_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, BCDC_CARDNAME_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;

}
/*
 * [Description  ] Get secure policy
 * [In Data      ]
 * [Response data] [Secure policy (4 bytes)]
 */
int wallet::GetSECPO(uint8_t OUT *SECPO)
{
	uint8_t rspdata[64] = { 0 };
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_GET_SECPO;

	cmd_le_dly_rty[0] = BCDC_SECPO_LEN & 0xFF;
	cmd_le_dly_rty[1] = (BCDC_SECPO_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, BCDC_SECPO_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	memcpy(SECPO, rspdata, BCDC_SECPO_LEN);
	return 0;
}

/*
 * [Description  ] Set secure policy
 * [In Data      ] [Secure policy (4 bytes)] [PIN (6 bytes)] [PIN challenge (16 bytes)]
 * [Response data] 
 */
int wallet::SetSECPO(uint8_t IN *SECPO, uint8_t IN *inputPIN, uint8_t IN *pinChlng)
{
	uint8_t hash[PINHASH_LEN] = { 0 };
	uint8_t pin_resp[PIN_RESP_LEN+16] = { 0 };
	uint8_t rspdata[64] = { 0 };
	uint8_t cmd_buf[CMD_LEN + BCDC_SECPO_LEN + PIN_RESP_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_SET_SECPO;
	cmd_buf[4] = (BCDC_SECPO_LEN + PIN_RESP_LEN) & 0xFF;
	cmd_buf[5] = ((BCDC_SECPO_LEN + PIN_RESP_LEN) >> 8) & 0xFF;

	hal_sha256_context_t context = {0};
	hal_sha256_init(&context);
	hal_sha256_append(&context, inputPIN, PIN_LEN);
	hal_sha256_end(&context, hash);

	//clear PIN
	memset(inputPIN, 0, PIN_LEN);

	//reg_resp = enc(challenge, pinHASH)

	hal_aes_buffer_t plain_text = {
	    .buffer = pinChlng,
	    .length = AUTHUTIL_CHLNG_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = hash,
	    .length = 32
	};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = pin_resp,
	    .length = PIN_RESP_LEN+16
	};


	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
	//ikvce_aes_key_initialize(hash, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(pin_resp, PIN_RESP_LEN, pinChlng, AUTHUTIL_CHLNG_LEN / IKVCE_BLOCK_SIZE_AES);

	//clear pinhash
	memset(hash, 0, PINHASH_LEN);

	memcpy(cmd_buf + CMD_LEN, SECPO, BCDC_SECPO_LEN);
	memcpy(cmd_buf + CMD_LEN + BCDC_SECPO_LEN, pin_resp, PIN_RESP_LEN);
	memcpy(cmd_buf + CMD_LEN + BCDC_SECPO_LEN + PIN_RESP_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + BCDC_SECPO_LEN + PIN_RESP_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

/* 
 * [Description  ] Bind Register Init
 * [Response data] [BRHandle (4 bytes)] [OTP (6 bytes)]
 */
int wallet::BindRegInit(uint8_t firstHost, uint8_t IN *HostCred, uint8_t IN *HostDesc, uint8_t OUT *rspdata)
{
	//sha256_ctx ctx;
	uint8_t cmd_buf[BIND_INIT_BUF_LEN] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_le_dly_rty[0] = (HANDLE_LEN + BIND_OTP_LEN) & 0xFF;
	cmd_le_dly_rty[1] = ((HANDLE_LEN + BIND_OTP_LEN) >> 8) & 0xFF;

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_REG_INIT;
	cmd_buf[2] = firstHost;
	cmd_buf[4] = (HST_CRED_LEN + HST_DESC_LEN + HAL_SHA256_DIGEST_SIZE) & 0xFF;
	cmd_buf[5] = ((HST_CRED_LEN + HST_DESC_LEN + HAL_SHA256_DIGEST_SIZE) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, HostCred, HST_CRED_LEN);
	memcpy(cmd_buf + CMD_LEN + HST_CRED_LEN, HostDesc, HST_DESC_LEN);

	/*
	//sha256(cmd_buf + CMD_LEN, HST_CRED_LEN + HST_DESC_LEN, cmd_buf + CMD_LEN + HST_CRED_LEN + HST_DESC_LEN);
	sha256_init(&ctx);
	sha256_update(&ctx, cmd_buf + CMD_LEN, HST_CRED_LEN);
	sha256_update(&ctx, cmd_buf + CMD_LEN + HST_CRED_LEN, HST_DESC_LEN);
	sha256_final(&ctx, cmd_buf + CMD_LEN + HST_CRED_LEN + HST_DESC_LEN);
	memcpy(cmd_buf + BIND_INIT_BUF_LEN - CMD_LE_DLY_RTY, cmd_le_dly_rty, CMD_LE_DLY_RTY);
	//dumpdata("Host hash", cmd_buf, buf_len);
	*/
	hal_sha256_context_t context = {0};
	hal_sha256_init(&context);
	hal_sha256_append(&context, cmd_buf + CMD_LEN, HST_CRED_LEN);
	hal_sha256_append(&context, cmd_buf + CMD_LEN + HST_CRED_LEN, HST_DESC_LEN);
	hal_sha256_end(&context, cmd_buf + CMD_LEN + HST_CRED_LEN + HST_DESC_LEN);
	memcpy(cmd_buf + BIND_INIT_BUF_LEN - CMD_LE_DLY_RTY, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, BIND_INIT_BUF_LEN, rspdata, HANDLE_LEN + BIND_OTP_LEN) != 0) {
		//Serial.println("BindRegInit Fail");
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Bind Register Chanllenge
 * [Response data] [Register Challenge (16 bytes)]
 */
int wallet::BindRegChallenge(uint8_t IN *brHandle, uint8_t OUT *RegChlng)
{
	uint8_t cmd_buf[BIND_CHLNG_BUF_LEN] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_le_dly_rty[0] = BIND_CHLNG_LEN & 0xFF;
	cmd_le_dly_rty[1] = (BIND_CHLNG_LEN >> 8) & 0xFF;

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_REG_CHLNG;
	cmd_buf[4] = HANDLE_LEN & 0xFF;
	cmd_buf[5] = (HANDLE_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, brHandle, HANDLE_LEN);
	memcpy(cmd_buf + BIND_CHLNG_BUF_LEN - CMD_LE_DLY_RTY, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, BIND_CHLNG_BUF_LEN, RegChlng, BIND_CHLNG_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

void sha256( uint8_t *message, uint8_t len, uint8_t *digest)
{
   	hal_sha256_context_t context = {0};
	hal_sha256_init(&context);
	hal_sha256_append(&context, message, len);
	hal_sha256_end(&context, digest);
}


/*
 * [Description  ] Bind Register Finish
 * [Response data] [HostID (1 byte)] [Bind State (1 byte)] [RegOtp (6 bytes)]
 */
int wallet::BindRegFinish(uint8_t IN *HostCred, uint8_t IN *brHandle, uint8_t IN *RegOtp, uint8_t IN *RegChlng, uint8_t OUT *rspdata)
{
	uint8_t cmd_buf[BIND_FINISH_BUF_LEN] = { 0 };
	uint8_t reg_resp[HAL_SHA256_DIGEST_SIZE] = { 0 };
	uint8_t key_matl[HST_CRED_LEN + BIND_OTP_LEN] = { 0 };        //key material
	uint8_t dev_key[HAL_SHA256_DIGEST_SIZE] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_le_dly_rty[0] = BIND_FIN_RSP_LEN & 0xFF;
	cmd_le_dly_rty[1] = (BIND_FIN_RSP_LEN >> 8) & 0xFF;

	//prepare the challenge response
	//calc devKey = hash(credential + retOtp)
	memcpy(key_matl, HostCred, HST_CRED_LEN);
	memcpy(key_matl + HST_CRED_LEN, RegOtp, BIND_OTP_LEN);
	sha256(key_matl, HST_CRED_LEN + BIND_OTP_LEN, dev_key);
	//reg_resp = enc(challenge, devKey)

    
	hal_aes_buffer_t plain_text = {
	    .buffer = RegChlng,
	    .length = 16
	};
	hal_aes_buffer_t key = {
	    .buffer = dev_key,
	    .length = HAL_SHA256_DIGEST_SIZE
	};
	uint8_t encrypted_buffer[16] = {0};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = reg_resp,
	    .length = HAL_SHA256_DIGEST_SIZE
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);

	/*
	dumpData("challenge", RegChlng, 16);
	dumpData("devKey", dev_key, 32);
	dumpData("reg_resp", reg_resp, BIND_RESP_LEN);
	*/

	//	ikvce_aes_key_initialize(dev_key, 32, MOP_ECB, ACT_ENC, NULL);
	//	ikvce_aes_crypt(reg_resp, BIND_RESP_LEN, RegChlng, 1);
	//dumpdata("reg_resp", reg_resp, BIND_RESP_LEN);
	memset(dev_key, 0, HAL_SHA256_DIGEST_SIZE);    //clear sensitive data

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_REG_FINISH;
	cmd_buf[4] = (HANDLE_LEN + BIND_RESP_LEN + PIN_RESP_LEN) & 0xFF;
	cmd_buf[5] = ((HANDLE_LEN + BIND_RESP_LEN + PIN_RESP_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, brHandle, HANDLE_LEN);
	memcpy(cmd_buf + CMD_LEN + HANDLE_LEN, reg_resp, BIND_RESP_LEN);
	//PIN_RESP currently not use
	memcpy(cmd_buf + BIND_FINISH_BUF_LEN - CMD_LE_DLY_RTY, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, BIND_FINISH_BUF_LEN, rspdata, BIND_FIN_RSP_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	//must store otp for bind login
	memcpy(rspdata + BIND_FIN_RSP_LEN, RegOtp, BIND_OTP_LEN);

	return 0;
}

/*
 * [Description  ] Bind Register Info
 * [Response data] [BindState (1 byte)] [HostDesc (64 bytes)]
 */
int wallet::BindRegInfo(uint8_t IN hostID, uint8_t OUT *RegInfo)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_le_dly_rty[0] = (1 + HST_DESC_LEN) & 0xFF;
	cmd_le_dly_rty[1] = ((1 + HST_DESC_LEN) >> 8) & 0xFF;

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_REG_INFO;
	cmd_buf[2] = hostID;
	
	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, RegInfo, 1 + HST_DESC_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}



/*
 * [Description  ] Bind Login Challenge
 * [Response data] [Login challenge (16 bytes)]
 */
int wallet::BindLoginChallenge(uint8_t IN hostID, uint8_t OUT *LoginChlng)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_le_dly_rty[0] = BIND_CHLNG_LEN & 0xFF;
	cmd_le_dly_rty[1] = (BIND_CHLNG_LEN >> 8) & 0xFF;

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_LOGIN_CHLNG;
	cmd_buf[2] = hostID;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, LoginChlng, BIND_CHLNG_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Bind Login
 * [Response data] 
 */
int wallet::BindLogin(uint8_t IN hostID, uint8_t IN *HostCred, uint8_t IN *RegOtp, uint8_t IN *LoginChlng, uint8_t OUT *rspdata)
{
	uint8_t cmd_buf[CMD_LEN + BIND_RESP_LEN + CMD_LE_DLY_RTY] = { 0 };
	uint8_t key_matl[HST_CRED_LEN + BIND_OTP_LEN] = { 0 };        //key material
	uint8_t dev_key[HAL_SHA256_DIGEST_SIZE] = { 0 };
	uint8_t login_resp[HAL_SHA256_DIGEST_SIZE] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_LOGIN;
	cmd_buf[2] = hostID;
	cmd_buf[4] = BIND_RESP_LEN & 0xFF;
	cmd_buf[5] = (BIND_RESP_LEN >> 8) & 0xFF;

	//prepare the challenge response
	//calc devKey = hash(credential + retOtp)
	memcpy(key_matl, HostCred, HST_CRED_LEN);
	memcpy(key_matl + HST_CRED_LEN, RegOtp, BIND_OTP_LEN);
	sha256(key_matl, HST_CRED_LEN + BIND_OTP_LEN, dev_key);
	//reg_resp = enc(challenge, devKey)


	hal_aes_buffer_t plain_text = {
	    .buffer = LoginChlng,
	    .length = 16
	};
	hal_aes_buffer_t key = {
	    .buffer = dev_key,
	    .length = HAL_SHA256_DIGEST_SIZE
	};
	uint8_t encrypted_buffer[16] = {0};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = login_resp,
	    .length = HAL_SHA256_DIGEST_SIZE
	};


	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);


	//ikvce_aes_key_initialize(dev_key, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(login_resp, BIND_RESP_LEN, LoginChlng, 1);


	//dumpdata("reg_resp", reg_resp, BIND_RESP_LEN);
	memset(dev_key, 0, HAL_SHA256_DIGEST_SIZE);    //clear sensitive data

	memcpy(cmd_buf + CMD_LEN, login_resp, BIND_RESP_LEN);
	memcpy(cmd_buf + CMD_LEN + BIND_RESP_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + BIND_RESP_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Bind Logout
 * [Response data]
 */
int wallet::BindLogout(void)
{
	uint8_t rspdata[64];
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_LOGOUT;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}


/*
 * [Description  ] PIN Challenge
 * [In Data      ]
 * [Response data] [CHLNG (16 bytes)]
 */
int wallet::PIN_CHLNG(bool IN PIN_AUTH, uint8_t OUT *chlng)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_PIN_CHLNG;
	cmd_buf[2] = PIN_AUTH;

	cmd_le_dly_rty[0] = AUTHUTIL_CHLNG_LEN & 0xFF;
	cmd_le_dly_rty[1] = (AUTHUTIL_CHLNG_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);
	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, chlng, AUTHUTIL_CHLNG_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

/*
 * [Description  ] Normal/AUTH mode PIN authentication
 * [In Data      ] [Respone of challenge (16bytes)]
 * [Response data] 
 */
int wallet::ModeAuth(uint8_t IN *inputPIN, uint8_t IN *pinChlng)
{
	hal_sha256_context_t ctx;
	uint8_t hash[PINHASH_LEN] = { 0 };
	uint8_t pin_resp[PIN_RESP_LEN+16] = { 0 };
	uint8_t rspdata[64];
	uint8_t cmd_buf[CMD_LEN + PIN_RESP_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };

	hal_sha256_init(&ctx);
	hal_sha256_append(&ctx, inputPIN, 6);
	hal_sha256_end(&ctx, hash);

	//reg_resp = enc(challenge, pinHASH)
	hal_aes_buffer_t plain_text = {
	    .buffer = pinChlng,
	    .length = AUTHUTIL_CHLNG_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = hash,
	    .length = 32
	};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = pin_resp,
	    .length = PIN_RESP_LEN+16
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
	//ikvce_aes_key_initialize(hash, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(pin_resp, PIN_RESP_LEN, pinChlng, AUTHUTIL_CHLNG_LEN / IKVCE_BLOCK_SIZE_AES);

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_PIN_AUTH;
	cmd_buf[4] = PIN_RESP_LEN & 0xFF;
	cmd_buf[5] = (PIN_RESP_LEN >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, pin_resp, PIN_RESP_LEN);
	memcpy(cmd_buf + CMD_LEN + PIN_RESP_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + PIN_RESP_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
* [Description  ] Change PIN
* [In Data      ] [old PIN (6bytes)] [new PIN (6bytes)]
* [Response data]
*/
int wallet::Change_PIN(uint8_t IN *oldPIN, uint8_t IN *newPIN)
{
	char *enc_str = "ENC";
	hal_sha256_context_t ctx;
	uint8_t credbuf[64];
	uint8_t bind_senck[BIND_SK_LEN] = { 0 };
	uint8_t oldhash[PINHASH_LEN] = { 0 };
	uint8_t enc_oldhash[PINHASH_LEN+16] = { 0 };
	uint8_t newhash[PINHASH_LEN] = { 0 };
	uint8_t enchash[PINHASH_LEN] = { 0 };
	uint8_t mac[MAC_LEN] = { 0 };
	uint8_t rspdata[64];
	uint8_t cmd_buf[CMD_LEN + PINHASH_LEN + PINHASH_LEN + MAC_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	
	hal_sha256_init(&ctx);
	hal_sha256_append(&ctx, oldPIN, 6);
	hal_sha256_end(&ctx, oldhash);

	hal_sha256_init(&ctx);
	hal_sha256_append(&ctx, newPIN, 6);
	hal_sha256_end(&ctx, newhash);

	//clear PIN
	memset(oldPIN, 0, 6);
	memset(newPIN, 0, 6);

	if (ReadCred(credbuf) != 0) {
		return -1;
	}

	derive_bind_session_key(host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, (uint8_t *)enc_str, strlen(enc_str), bind_senck);
	//encrypt old pin hash

	hal_aes_buffer_t plain_text = {
	    .buffer = oldhash,
	    .length = PINHASH_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = bind_senck,
	    .length = 32
	};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = enc_oldhash,
	    .length = PINHASH_LEN+16
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);

	//ikvce_aes_key_initialize(bind_senck, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(enc_oldhash, PINHASH_LEN, oldhash, PINHASH_LEN / IKVCE_BLOCK_SIZE_AES);

	//reg_resp = enc(newhash, oldhash)
	plain_text = {
	    .buffer = newhash,
	    .length = PINHASH_LEN
	};
	key = {
	    .buffer = oldhash,
	    .length = PINHASH_LEN
	};
	encrypted_text = {
	    .buffer = enchash,
	    .length = PINHASH_LEN+16
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);

	//ikvce_aes_key_initialize(oldhash, PINHASH_LEN, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(enchash, PINHASH_LEN, newhash, PINHASH_LEN / IKVCE_BLOCK_SIZE_AES);

	bind_session_mac(enchash, PINHASH_LEN, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);

	//clear PINHASH
	memset(newhash, 0, PINHASH_LEN);
	memset(oldhash, 0, PINHASH_LEN);

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_PIN_CHANGE;
	cmd_buf[4] = (PINHASH_LEN + PINHASH_LEN + MAC_LEN) & 0xFF;
	cmd_buf[5] = ((PINHASH_LEN + PINHASH_LEN + MAC_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, enc_oldhash, PINHASH_LEN);
	memcpy(cmd_buf + CMD_LEN + PINHASH_LEN, enchash, PINHASH_LEN);
	memcpy(cmd_buf + CMD_LEN + PINHASH_LEN + PINHASH_LEN, mac, MAC_LEN);
	memcpy(cmd_buf + CMD_LEN + PINHASH_LEN + PINHASH_LEN + MAC_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + PINHASH_LEN +  PINHASH_LEN + MAC_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Bind back to no host state (clear host and wallet)
 * [In Data      ] [PIN (6 bytes)] [PINCHLNG (16 bytes)]
 * [Response data]
 */
int wallet::BindBackToNoHost(uint8_t IN *pin,uint8_t IN *pinChlng)
{
	hal_sha256_context_t ctx;

	char *enc_str = "ENC";
	uint8_t credbuf[64] = { 0 };
	uint8_t bind_senck[BIND_SK_LEN] = { 0 };
	uint8_t hash[PINHASH_LEN] = { 0 };
	uint8_t pin_resp[PIN_RESP_LEN+16] = { 0 };
	uint8_t rspdata[64];
	uint8_t cmd_buf[CMD_LEN + PIN_RESP_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	
	//should caculate pin resp and pin hash for different login status & perso setting
	//no need pin rsp & hash in following condition : disconn state, persoed and (secpo & SECPO_MASK_PIN == 0)
	//if login, the pinhash need to encrpyt by session key and will be the new PIN.
	hal_sha256_init(&ctx);
	hal_sha256_append(&ctx, pin, PIN_LEN);
	hal_sha256_end(&ctx, hash);
	memset(pin, 0, PIN_LEN);

	//reg_resp = enc(challenge, pinHASH)
	hal_aes_buffer_t plain_text = {
	    .buffer = pinChlng,
	    .length = PINHASH_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = hash,
	    .length = 32
	};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = pin_resp,
	    .length = PINHASH_LEN+16
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);

	//ikvce_aes_key_initialize(hash, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(pin_resp, PIN_RESP_LEN, pinChlng, AUTHUTIL_CHLNG_LEN/IKVCE_BLOCK_SIZE_AES);
	memset(hash, 0, PINHASH_LEN);

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_BIND_BACK_NOHOST;
	cmd_buf[4] = (PIN_RESP_LEN) & 0xFF;
	cmd_buf[5] = (PIN_RESP_LEN >> 8) & 0xFF;
	
	memcpy(cmd_buf + CMD_LEN, pin_resp, PIN_RESP_LEN);
	memcpy(cmd_buf + CMD_LEN + PIN_RESP_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + PIN_RESP_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	
	return 0;
}

/*
 * [Description  ] Derive bind session key
 * [In Data      ] Host credential, Bind register OTP, Login challenge, material
 * [Response data] Session Key
 */
int wallet::derive_bind_session_key(uint8_t IN *HostCred, uint8_t IN *RegOtp, uint8_t IN *LoginChlng,
								uint8_t IN *mtrl, uint32_t IN mtrl_len, uint8_t OUT *sess_key)
{

	uint8_t dev_key[HAL_SHA256_DIGEST_SIZE] = { 0 };
	//calc devKey = hash(credential + retOtp)


	hal_sha256_context_t ctx_dk,ctx_sk;
	hal_sha256_init(&ctx_dk);
	hal_sha256_append(&ctx_dk, HostCred, HST_CRED_LEN);
	hal_sha256_append(&ctx_dk, RegOtp, BIND_OTP_LEN);
	hal_sha256_end(&ctx_dk,dev_key);

	//dumpData("HostCred", HostCred, HST_CRED_LEN);
	//dumpData("RegOtp", RegOtp, BIND_OTP_LEN);
	//dumpData("dev_key", dev_key, 32);

	hal_sha256_init(&ctx_sk);
	hal_sha256_append(&ctx_sk, LoginChlng, BIND_CHLNG_LEN);
	hal_sha256_append(&ctx_sk, dev_key, HAL_SHA256_DIGEST_SIZE);
	if (mtrl_len > 0) {
		hal_sha256_append(&ctx_sk, mtrl, mtrl_len);
	}
	hal_sha256_end(&ctx_sk,sess_key);

	//dumpData("LoginChlng", LoginChlng, BIND_CHLNG_LEN);
	//dumpData("dev_key", dev_key, 32);
	//dumpData("mtrl", mtrl, mtrl_len);
	//dumpData("sess_key", sess_key, 32);
	//



	memset(dev_key, 0, HAL_SHA256_DIGEST_SIZE);
	return 0;
}
/*
 * [Description  ] Calculate data MAC value by binding session
 * [In Data      ] Data, Host credential, Bind register OTP, Login challenge
 * [Response data] MAC
 */
int wallet::bind_session_mac(uint8_t IN *data, uint32_t data_len, uint8_t IN *HostCred,
						uint8_t IN *RegOtp, uint8_t IN *LoginChlng, uint8_t OUT *mac)
{
	char *mac_str = "MAC";
	uint8_t key_matl[HST_CRED_LEN + BIND_OTP_LEN] = { 0 };        //key material
	uint8_t dev_key[HAL_SHA256_DIGEST_SIZE] = { 0 };
	uint8_t bind_smack[BIND_SK_LEN];


	hmacsha256_ctx hmacctx;

	derive_bind_session_key(HostCred, RegOtp, LoginChlng, (uint8_t *)mac_str, strlen(mac_str), bind_smack);
	/*
	//calc devKey = hash(credential + retOtp)
	memcpy(key_matl, HostCred, HST_CRED_LEN);
	memcpy(key_matl + HST_CRED_LEN, RegOtp, BIND_OTP_LEN);
	sha256(key_matl, HST_CRED_LEN + BIND_OTP_LEN, dev_key);

	//calc hmac key
	sha256_init(&ctx);
	sha256_update(&ctx, LoginChlng, BIND_CHLNG_LEN);
	sha256_update(&ctx, dev_key, HAL_SHA256_DIGEST_SIZE);
	sha256_update(&ctx, (uint8_t *)mac_str, strlen(mac_str));
	sha256_final(&ctx, bind_smack);
	*/
	//calc data MAC value



	hmacsha256_init(&hmacctx, bind_smack, BIND_SK_LEN);
	hmacsha256_update(&hmacctx, data, data_len);
	hmacsha256_final(&hmacctx, mac, MAC_LEN);

	//dumpData("bind_smack",bind_smack,BIND_SK_LEN);
	//dumpData("data",data,data_len);
	//dumpData("mac",mac,MAC_LEN);

	memset(dev_key, 0, HAL_SHA256_DIGEST_SIZE);    //clear sensitive data
	memset(bind_smack, 0, BIND_SK_LEN);    //clear sensitive data

	return 0;
}

int wallet::perso_set_data(uint8_t IN *HostCred,uint8_t IN *RegOtp, uint8_t IN *LoginChlng, uint8_t OUT *rspdata)
{
	uint8_t pd[BCDC_SECPO_LEN] = { 0 };    //perso data
	uint8_t mac[MAC_LEN] = { 0 };
	uint8_t cmd_buf[CMD_LEN + BCDC_SECPO_LEN + MAC_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x05, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_PERSO_SET_DATA;
	cmd_buf[2] = 0;    //PD_ID
	cmd_buf[4] = (BCDC_SECPO_LEN + MAC_LEN) & 0xFF;
	cmd_buf[5] = ((BCDC_SECPO_LEN + MAC_LEN) >> 8) & 0xFF;

	bind_session_mac(pd, BCDC_SECPO_LEN, HostCred, RegOtp, LoginChlng, mac);

	memcpy(cmd_buf + CMD_LEN, pd, BCDC_SECPO_LEN);
	memcpy(cmd_buf + CMD_LEN + BCDC_SECPO_LEN, mac, MAC_LEN);
	memcpy(cmd_buf + CMD_LEN + BCDC_SECPO_LEN + MAC_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + BCDC_SECPO_LEN + MAC_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

int wallet::perso_confirm(uint8_t OUT *rspdata)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x40, 0x01,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_PERSO_CONFIRM;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}
/*
 * [Description  ] perso set PIN of normal mode
 * [In Data      ] PIN (6 bytes)
 * [Response data]
 */
int wallet::perso_set_pin_n(uint8_t IN *PIN)
{
	char *enc_str = "ENC";
	uint8_t rspdata[64] = { 0 };
	uint8_t credbuf[64] = { 0 };
	uint8_t bind_senck[BIND_SK_LEN] = { 0 };
	uint8_t cmd_buf[CMD_LEN + PINHASH_LEN + CMD_LE_DLY_RTY] = { 0 };
	uint8_t pinhash[PINHASH_LEN] = { 0 };

	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x02, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_PERSO_SET_PIN_N;
	cmd_buf[4] = PINHASH_LEN & 0xFF;
	cmd_buf[5] = (PINHASH_LEN >> 8) & 0xFF;

	//raw pin to hash
	
	/*sha256_init(&ctx);
	sha256_update(&ctx, PIN, 6);
	sha256_final(&ctx, pinhash);
	*/
	hal_sha256_context_t context = {0};
	hal_sha256_init(&context);
	hal_sha256_append(&context, PIN, 6);
	hal_sha256_end(&context, pinhash);

	//clear PIN
	memset(PIN, 0, 6);

	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	derive_bind_session_key(host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, (uint8_t *)enc_str, strlen(enc_str), bind_senck);
	//encrypt pinhash
	//ikvce_aes_key_initialize(bind_senck, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(cmd_buf + CMD_LEN, PINHASH_LEN, pinhash, PINHASH_LEN / IKVCE_BLOCK_SIZE_AES);

	hal_aes_buffer_t plain_text = {
	    .buffer = pinhash,
	    .length = PINHASH_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = bind_senck,
	    .length = 32
	};
	uint8_t encrypted_buffer[48] = {0};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = encrypted_buffer,
	    .length = PINHASH_LEN+16
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
	memcpy(cmd_buf + CMD_LEN,encrypted_buffer,PINHASH_LEN);

	//ikvce_aes_key_initialize(bind_senck, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(emkseed, BIP39_SEED_LEN, mkseed, BIP39_SEED_LEN / IKVCE_BLOCK_SIZE_AES);



	memcpy(cmd_buf + CMD_LEN + PINHASH_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + PINHASH_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

//int hdw_init_wallet(uint8_t IN *HostCred, uint8_t IN *RegOtp, uint8_t IN *LoginChlng, uint8_t IN *hdwName, uint8_t OUT *rspdata)
int wallet::hdw_init_wallet(uint8_t IN *hdwName, uint8_t OUT *rspdata)
{
	char *enc_str = "ENC";
	uint8_t credbuf[64] = { 0 };
	uint8_t key_matl[HST_CRED_LEN + BIND_OTP_LEN] = { 0 };        //key material
	uint8_t dev_key[HAL_SHA256_DIGEST_SIZE] = { 0 };
	uint8_t bind_senck[BIND_SK_LEN] = { 0 };
	uint8_t mkseed[BIP39_SEED_LEN] = { 0x78, 0x31, 0x89, 0x9b, 0xc7, 0x0f, 0xdd, 0x60,
									   0x6d, 0x9a, 0xe6, 0xac, 0x44, 0xe2, 0x52, 0x22,
									   0xe1, 0x7f, 0xd8, 0xc8, 0xef, 0x6a, 0x04, 0x1b,
									   0xfe, 0xe7, 0xe3, 0xd8, 0x7e, 0x99, 0x22, 0xc1,
									   0xed, 0x50, 0xd0, 0xfd, 0xa1, 0x8d, 0xd0, 0xc6,
									   0x21, 0xb9, 0xff, 0x4b, 0xf6, 0x16, 0x40, 0x5c,
									   0x0e, 0x57, 0xd0, 0xae, 0xe7, 0xdd, 0xfb, 0x6f,
									   0xba, 0xe9, 0x4d, 0x4a, 0x95, 0xd4, 0x9d, 0xb5, };

	uint8_t emkseed[BIP39_SEED_LEN+16] = { 0 };
	uint8_t mac[MAC_LEN] = { 0 };
	uint8_t cmd_buf[INIT_WALLET_BUF_LEN] = { 0 };
	//little endian, response length 2bytes, delay time 4byt es(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x18, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_INIT_WALLET;
	cmd_buf[4] = (HDW_NAME_LEN + BIP39_SEED_LEN + MAC_LEN) & 0xFF;
	cmd_buf[5] = ((HDW_NAME_LEN + BIP39_SEED_LEN + MAC_LEN) >> 8) & 0xFF;
	//cmd_le_dly_rty[0] = BIP39_SEED_LEN;

	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	derive_bind_session_key(host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, (uint8_t *)enc_str, strlen(enc_str), bind_senck);
	//dumpdata("bind_senck", bind_senck, BIND_SK_LEN);
	//encrypt mkseed

	hal_aes_buffer_t plain_text = {
	    .buffer = mkseed,
	    .length = BIP39_SEED_LEN
	};
	hal_aes_buffer_t key = {
	    .buffer = bind_senck,
	    .length = 32
	};
	uint8_t encrypted_buffer[32] = {0};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = emkseed,
	    .length = BIP39_SEED_LEN+16
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);


	//ikvce_aes_key_initialize(bind_senck, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(emkseed, BIP39_SEED_LEN, mkseed, BIP39_SEED_LEN / IKVCE_BLOCK_SIZE_AES);

	bind_session_mac(emkseed, BIP39_SEED_LEN, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);
	//dumpdata("emkseed", emkseed, BIP39_SEED_LEN);
	//dumpdata("mac", mac, MAC_LEN);
	memcpy(cmd_buf + CMD_LEN, hdwName, HDW_NAME_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_NAME_LEN, emkseed, BIP39_SEED_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_NAME_LEN + BIP39_SEED_LEN, mac, MAC_LEN);
	memcpy(cmd_buf + INIT_WALLET_BUF_LEN - CMD_LE_DLY_RTY, cmd_le_dly_rty, CMD_LE_DLY_RTY);
	//dumpdata("cmd_buf", cmd_buf, INIT_WALLET_BUF_LEN);
	if (Kv_Apdu_Transparent(cmd_buf, INIT_WALLET_BUF_LEN, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Initialize HDW from mnemonic w/ or wo/ passphrase
                   You can choose to initalize wallet from mnemoinc wo/ passphrase and then
                   initialize another wallet from the same mnemonic w/ passphrase.
                   With this way, you will have two seeds to create accounts.
                   Or you can pass mnemonic and passphrase in the same time to generate one seed.
 * [In Data      ] HDW name (32bytes), passphrase, passphrase length, mnemonic, mnemonic length
 * [Response data]
 */
int wallet::hdw_init_wallet_words(uint8_t IN *hdwName, char IN *passphrase, uint8_t IN passphr_len, char IN *mnemonic, uint8_t IN mnemonic_len, uint8_t OUT *rspdata)
{
	char *enc_str = "ENC";
	uint8_t credbuf[64] = { 0 };
	uint8_t key_matl[HST_CRED_LEN + BIND_OTP_LEN] = { 0 };        //key material
	uint8_t dev_key[HAL_SHA256_DIGEST_SIZE] = { 0 };
	uint8_t bind_senck[BIND_SK_LEN] = { 0 };
	uint8_t raw_wrods[PASSPHRASE_MAX_LEN + MNEMONIC_LEN] = { 0 };
	uint8_t encr_wrods[PASSPHRASE_MAX_LEN + MNEMONIC_LEN+16] = { 0 };
	uint8_t mac[MAC_LEN] = { 0 };

	uint8_t cmd_buf[INIT_WALLET_MNE_BUF_LEN] = { 0 };
	//little endian, response length 2bytes, delay time 4byt es(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x00, 0x90,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_INIT_WALLET_WORDS;
	cmd_buf[2] = passphr_len;
	cmd_buf[3] = mnemonic_len;
	cmd_buf[4] = (HDW_NAME_LEN + PASSPHRASE_MAX_LEN + MNEMONIC_LEN + MAC_LEN) & 0xFF;
	cmd_buf[5] = ((HDW_NAME_LEN + PASSPHRASE_MAX_LEN + MNEMONIC_LEN + MAC_LEN) >> 8) & 0xFF;


	//prepare raw data to encrypt
	memcpy(raw_wrods, passphrase, passphr_len);
	memcpy(raw_wrods+ passphr_len, mnemonic, mnemonic_len);

	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	//dumpData("credbuf", credbuf, 1 + BIND_OTP_LEN+BIND_CHLNG_LEN);

	derive_bind_session_key(host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, (uint8_t *)enc_str, strlen(enc_str), bind_senck);
	//encrypt mkseed

	hal_aes_buffer_t plain_text = {
	    .buffer = raw_wrods,
	    .length = (PASSPHRASE_MAX_LEN + MNEMONIC_LEN)
	};
	hal_aes_buffer_t key = {
	    .buffer = bind_senck,
	    .length = HAL_SHA256_DIGEST_SIZE
	};
	uint8_t encrypted_buffer[16] = {0};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = encr_wrods,
	    .length = PASSPHRASE_MAX_LEN + MNEMONIC_LEN+16
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);
	//dumpData("raw_wrods", raw_wrods, (PASSPHRASE_MAX_LEN + MNEMONIC_LEN));
	//dumpData("bind_senck", bind_senck, BIND_SK_LEN);
	//dumpData("encr_wrods", encr_wrods, PASSPHRASE_MAX_LEN + MNEMONIC_LEN);
	//ikvce_aes_key_initialize(bind_senck, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(encr_wrods, PASSPHRASE_MAX_LEN + MNEMONIC_LEN, raw_wrods, (PASSPHRASE_MAX_LEN + MNEMONIC_LEN) / IKVCE_BLOCK_SIZE_AES);



	bind_session_mac(encr_wrods, PASSPHRASE_MAX_LEN + MNEMONIC_LEN, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);

	memcpy(cmd_buf + CMD_LEN, hdwName, HDW_NAME_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_NAME_LEN, encr_wrods, PASSPHRASE_MAX_LEN + MNEMONIC_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_NAME_LEN + PASSPHRASE_MAX_LEN + MNEMONIC_LEN, mac, MAC_LEN);
	memcpy(cmd_buf + INIT_WALLET_MNE_BUF_LEN - CMD_LE_DLY_RTY, cmd_le_dly_rty, CMD_LE_DLY_RTY);
	//dumpdata("cmd_buf", cmd_buf, INIT_WALLET_MNE_BUF_LEN);
	//if (Kv_Apdu_Transparent(cmd_buf, INIT_WALLET_MNE_BUF_LEN, rspdata, 0) != 0) {
	if (Kv_Apdu_Transparent(cmd_buf, INIT_WALLET_MNE_BUF_LEN, rspdata, 0) != 0) {
		Serial.println("hdw_init_wallet_words failed\n");
		return -1;
	}
	return 0;
}


/*
 * [Description  ] Query HD wallet info
 * [Response data] HDW status or name or account pointer or all
 */
int wallet::hdw_qry_wa_info(uint8_t OUT *rspdata)
{
	uint16_t info_len[4] = { 1, 32, 4, 37 };
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_QRY_WA_INFO;
	//HDW status, name, account pointer or all
	//status : INACTIVE, WAITACTIVE, ACTIVE
	cmd_buf[2] = HDW_INFO_NUM;

	cmd_le_dly_rty[0] = info_len[cmd_buf[2]] & 0xFF;
	cmd_le_dly_rty[1] = (info_len[cmd_buf[2]] >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, info_len[cmd_buf[2]]) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Create HDW account
 * [IN Data      ] ACCNAME(32 bytes), ACC_PURPOSE(4 bytes, little - endian), ACC_COINTYPE(4 bytes, little - endian), ACCID(4 bytes, little - endian)
 * [Note         ] it may take about 15~20 seconds.
 */
int wallet::hdw_create_account(uint8_t IN *accName, uint8_t IN *accPurpose, uint8_t IN *accCoinType, uint8_t IN *accID, uint8_t OUT *rspdata)
{
	uint8_t cmd_buf[HDW_CREATE_ACC_BUF_LEN] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x20, 0x03,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_CREATE_ACCOUNT;
	cmd_buf[4] = (HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_ID_LEN) & 0xFF;
	cmd_buf[5] = ((HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_ID_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, accPurpose, HDW_ACC_PURPOSE_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_PURPOSE_LEN, accCoinType, HDW_ACC_COINTYPE_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN, accID, HDW_ACC_ID_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_ID_LEN, accName, HDW_ACCNAME_LEN);
	memcpy(cmd_buf + HDW_CREATE_ACC_BUF_LEN - CMD_LE_DLY_RTY, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, HDW_CREATE_ACC_BUF_LEN, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

/*
 * [Description  ] Query HDW account info
 * [IN Data      ] ACC_ID (4 bytes, little - endian)
 * [Response data] HDW_ACC_INFO_ALL : ACC_NAME(32 bytes), ACC_PURPOSE(4 bytes), ACC_COINTYPE(4 bytes), 
 *									  ACC_BAL(8 bytes), ACC_EXTKPTR(4 bytes), ACC_INTKPTR(4 bytes), XCHS ACC_BAL(8 bytes)
 */
int wallet::hdw_qry_acc_info(uint8_t IN *accID, uint8_t OUT *rspdata)
{
	uint8_t cmd_buf[CMD_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_QRY_ACC_INFO;
	cmd_buf[2] = HDW_ACCINFO_NUM;
	cmd_buf[4] = HDW_ACC_ID_LEN & 0xFF;
	cmd_buf[5] = (HDW_ACC_ID_LEN >> 8) & 0xFF;

	cmd_le_dly_rty[0] = HDW_ACC_INFO_ALL_LEN & 0xFF;
	cmd_le_dly_rty[1] = ((HDW_ACC_INFO_ALL_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, accID, HDW_ACC_ID_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_ID_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);


	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY, rspdata, HDW_ACC_INFO_ALL_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Set HDW account info
 * [IN Data      ] INFO_ID(1 byte) , ACC_ID(4 bytes, little - endian), ACCINFO(variable length)
 * [Response data] 
 */
int wallet::hdw_set_acc_info(uint8_t IN InfoID, uint8_t IN *accID, uint8_t *accInfo,uint8_t OUT *rspdata)
{
	uint16_t acc_info_len[HDW_ACCINFO_NUM] = { HDW_ACCNAME_LEN,
											   HDW_ACC_PURPOSE_LEN, HDW_ACC_COINTYPE_LEN,
											   HDW_ACC_BAL_LEN,
											   HDW_ACC_KPTR_LEN, HDW_ACC_KPTR_LEN,
											   HDW_ACC_BAL_LEN, };
	uint8_t mac[MAC_LEN] = { 0 };
	uint8_t credbuf[64] = { 0 };
	uint8_t cmd_buf[CMD_LEN + HDW_ACC_ID_LEN + HDW_ACCNAME_LEN + MAC_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x05, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_SET_ACC_INFO;
	cmd_buf[2] = InfoID;
	cmd_buf[4] = (HDW_ACC_ID_LEN + acc_info_len[InfoID] + MAC_LEN) & 0xFF;
	cmd_buf[5] = ((HDW_ACC_ID_LEN + acc_info_len[InfoID] + MAC_LEN) >> 8) & 0xFF;

	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	bind_session_mac(accInfo, acc_info_len[InfoID], host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);

	memcpy(cmd_buf + CMD_LEN, accID, HDW_ACC_ID_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_ID_LEN, accInfo, acc_info_len[InfoID]);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_ID_LEN + acc_info_len[InfoID], mac, MAC_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_ID_LEN + acc_info_len[InfoID] + MAC_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);
	
	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + HDW_ACC_ID_LEN + acc_info_len[InfoID] + MAC_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] Query HDW account key info
 * [IN Data      ] InfoID(1byte), KeyChainID(0:external; 1:internal)(1byte), ACCID(4 bytes, little - endian). KEYID(4 bytes, little - endian)
 * [Response data] KeyInfo(variable length), MAC(32 bytes)
 */
int wallet::hdw_qry_acc_key_info(uint8_t IN InfoID, uint8_t IN KCID, uint8_t IN *accID, uint8_t IN *keyID, uint8_t OUT *rspdata)
{
	uint8_t credbuf[64] = { 0 };
	uint8_t mac[MAC_LEN];
	uint16_t key_info_len[3] = { BC_BINADDR_LEN, HDW_KPUB_LEN, HDW_ACCKPUB_LEN };
	uint8_t cmd_buf[CMD_LEN + HDW_ACC_ID_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x00, 0x03,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_QRY_ACC_KEYINFO;
	cmd_buf[2] = InfoID;
	cmd_buf[3] = KCID;
	cmd_buf[4] = (HDW_ACC_ID_LEN + HDW_ACC_ID_LEN) & 0xFF;
	cmd_buf[5] = ((HDW_ACC_ID_LEN + HDW_ACC_ID_LEN) >> 8) & 0xFF;
	cmd_le_dly_rty[0] = (key_info_len[InfoID] + MAC_LEN) & 0xFF;
	cmd_le_dly_rty[1] = ((key_info_len[InfoID] + MAC_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, accID, HDW_ACC_ID_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_ID_LEN, keyID, HDW_ACC_ID_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_ID_LEN + HDW_ACC_ID_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + HDW_ACC_ID_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY, rspdata, key_info_len[InfoID] + MAC_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	//check MAC
	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	bind_session_mac(rspdata, key_info_len[InfoID], host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);
	//dumpdata("acc_key_info_mac", mac, MAC_LEN);
	if (memcmp(rspdata + key_info_len[InfoID], mac, MAC_LEN) != 0) {
		printf("%s check MAC failed\n", __FUNCTION__);
		return -2;
	}

	return 0;
}

/*
 * [Description  ] HDW generate entropy for creating mnemonic sentence (MS) 
 * [IN Data      ] entropy length (bits)
 * [Response data] entropy(variable length), MAC(32 bytes)
 */
int wallet::hdw_gen_entropy(uint16_t IN entropy_bit_len, uint8_t OUT *rspdata)
{
	uint8_t entropy_byte_len;
	uint8_t credbuf[64] = { 0 };
	uint8_t mac[MAC_LEN];
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x03, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_GEN_ENTROPY;
	cmd_buf[2] = entropy_bit_len & 0xFF;
	cmd_buf[3] = (entropy_bit_len >> 8) & 0xFF;
	
	entropy_byte_len = entropy_bit_len / 8;
	cmd_le_dly_rty[0] = (entropy_byte_len + MAC_LEN) & 0xFF;
	cmd_le_dly_rty[1] = ((entropy_byte_len + MAC_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, entropy_byte_len + MAC_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	//check MAC
	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	bind_session_mac(rspdata, entropy_byte_len, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);
	//dumpdata("acc_key_info_mac", mac, MAC_LEN);
	if (memcmp(rspdata + entropy_byte_len, mac, MAC_LEN) != 0) {
		printf("%s check MAC failed\n", __FUNCTION__);
		return -2;
	}

	return 0;
}

/*
* [Description  ] HDW mnemonic to mkseed
* [IN Data      ] passphrase (variable legth, max 16 bytes), mnemoinc (variable length)
* [Response data] mkseed(64 bytes), MAC(32 bytes)
*/
int wallet::hdw_mnemonic_to_seed(char IN *passphrase, char IN *mnemoinc, uint8_t OUT *rspdata)
{
	uint16_t passphrase_len;
	uint16_t total_len;
	uint8_t credbuf[64] = { 0 };
	uint8_t mac[MAC_LEN];
	uint8_t cmd_buf[CMD_LEN + PASSPHRASE_MAX_LEN + MNEMONIC_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x00, 0x86,
											   0x01, 0x00, };

	passphrase_len = strlen(passphrase);
	total_len = passphrase_len + strlen(mnemoinc);
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_MNEMONIC_TO_SEED;
	cmd_buf[2] = passphrase_len & 0xFF;
	cmd_buf[3] = (passphrase_len >> 8) & 0xFF;
	cmd_buf[4] = total_len & 0xFF;
	cmd_buf[5] = (total_len >> 8) & 0xFF;

	cmd_le_dly_rty[0] = (BIP39_SEED_LEN + MAC_LEN) & 0xFF;
	cmd_le_dly_rty[1] = ((BIP39_SEED_LEN + MAC_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, passphrase, passphrase_len);
	memcpy(cmd_buf + CMD_LEN + passphrase_len, mnemoinc, total_len - passphrase_len);
	memcpy(cmd_buf + CMD_LEN + total_len, cmd_le_dly_rty, CMD_LE_DLY_RTY);


	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + total_len + CMD_LE_DLY_RTY, rspdata, BIP39_SEED_LEN + MAC_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	//check MAC
	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	bind_session_mac(rspdata, BIP39_SEED_LEN, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);
	//dumpdata("acc_key_info_mac", mac, MAC_LEN);
	if (memcmp(rspdata + BIP39_SEED_LEN, mac, MAC_LEN) != 0) {
		printf("%s check MAC failed\n", __FUNCTION__);
		return -2;
	}

	return 0;
}

/*
 * [Description  ] Get next transaction address
 * [IN Data      ] ACCID(4 bytes, little - endian), KeyChainID(0:external; 1:internal)(1byte)
 * [Response data] KEYID(4 bytes, little - endian), binary address (25 bytes), MAC (32bytes)
 */
int wallet::hdw_next_trx_addr(uint8_t IN KCID, uint8_t IN *accID, uint8_t OUT *rspdata)
{
	uint8_t credbuf[64] = { 0 };
	uint8_t cmp_mac[MAC_LEN] = { 0 };
	uint8_t cmd_buf[CMD_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x00, 0x03,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_HDW_NEXT_TRX_ADDR;
	cmd_buf[2] = KCID;
	cmd_buf[4] = HDW_ACC_ID_LEN & 0xFF;
	cmd_buf[5] = (HDW_ACC_ID_LEN >> 8) & 0xFF;
	cmd_le_dly_rty[0] = (HDW_ACC_ID_LEN + BC_BINADDR_LEN  + MAC_LEN) & 0xFF;
	cmd_le_dly_rty[1] = ((HDW_ACC_ID_LEN + BC_BINADDR_LEN + MAC_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, accID, HDW_ACC_ID_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_ID_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY, rspdata, HDW_ACC_ID_LEN + BC_BINADDR_LEN + MAC_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	//check MAC
	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	bind_session_mac(rspdata, HDW_ACC_ID_LEN + BC_BINADDR_LEN, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, cmp_mac);

	if (memcmp(rspdata + HDW_ACC_ID_LEN + BC_BINADDR_LEN, cmp_mac, MAC_LEN) != 0) {
		printf("%s MAC doesn't match\n", __FUNCTION__);
		return -2;
	}

	return 0;
}

/*
 * [Description  ] HDW prepare transanction data to sign
 * [IN Data      ] InputID(1bytes), KeyChainID(0:external; 1:internal)(1byte), ACCID(4 bytes, little - endian), KeyID(4 bytes, little - endian)
                   Balance(8 bytes, big - endian), TrxHash(32 bytes)
 * [Response data] 
 */
int wallet::hdw_prep_trx_sign(uint8_t IN inputID, uint8_t IN kcID, uint8_t IN *accID, uint8_t IN *keyID,
						uint8_t IN *balnc, uint8_t IN *trxHash, uint8_t OUT *rspdata)
{
#pragma pack(push, 1)
	struct _pre_trx_s {
		uint8_t cmd_buf[CMD_LEN];
		uint8_t accID[HDW_ACC_ID_LEN];
		uint8_t keyID[HDW_ACC_ID_LEN];
		uint8_t balnc[HDW_ACC_BAL_LEN];
		uint8_t trxHash[HAL_SHA256_DIGEST_SIZE];
		uint8_t mac[MAC_LEN];
		uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY];
	}pre_trx_s;
#pragma pack(pop)
	uint16_t pre_trx_buf_len ,mac_data_len;
	uint8_t credbuf[64] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty_init[CMD_LE_DLY_RTY] = { 0x00, 0x00,
													0x05, 0x00,
													0x01, 0x00, };
	pre_trx_buf_len = sizeof(pre_trx_s);
	pre_trx_s.cmd_buf[0] = CMD_CLA;
	pre_trx_s.cmd_buf[1] = INS_HDW_PREP_TRX_SIGN;
	pre_trx_s.cmd_buf[2] = inputID;
	pre_trx_s.cmd_buf[3] = kcID;
	pre_trx_s.cmd_buf[4] = (pre_trx_buf_len - CMD_LEN - CMD_LE_DLY_RTY) & 0xFF;
	pre_trx_s.cmd_buf[5] = ((pre_trx_buf_len - CMD_LEN - CMD_LE_DLY_RTY) >> 8) & 0xFF;

	memcpy(pre_trx_s.accID, accID, HDW_ACC_ID_LEN);
	memcpy(pre_trx_s.keyID, keyID, HDW_ACC_ID_LEN);
	memcpy(pre_trx_s.balnc, balnc, HDW_ACC_BAL_LEN);
	memcpy(pre_trx_s.trxHash, trxHash, HAL_SHA256_DIGEST_SIZE);

	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	mac_data_len = pre_trx_buf_len - CMD_LEN - CMD_LE_DLY_RTY - MAC_LEN;
	bind_session_mac(pre_trx_s.accID, mac_data_len, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, pre_trx_s.mac);
	
	memcpy(pre_trx_s.cmd_le_dly_rty, cmd_le_dly_rty_init, CMD_LE_DLY_RTY);
	dumpData("pre_trx_s", (uint8_t *)pre_trx_s.cmd_buf, pre_trx_buf_len);

	if (Kv_Apdu_Transparent((uint8_t *)pre_trx_s.cmd_buf, pre_trx_buf_len, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}

/*
 * [Description  ] HDW prepare transanction data to sign
 * [IN Data      ] TRX_AMOUNT(8 byte), OUTADDR(34 bytes)
 * [Response data] OTP (6 bytes)
 */
int wallet::hdw_trx_sign_begin(uint8_t IN *trxAmount, uint8_t IN *outAddr, uint8_t OUT *otp)
{
	char *enc_str = "ENC";
	uint8_t credbuf[64] = { 0 };
	uint8_t dev_key[HAL_SHA256_DIGEST_SIZE] = { 0 };
	uint8_t bind_senck[BIND_SK_LEN] = { 0 };
	uint8_t outAddr_pad[ENC_OUTADDR_LEN] = { 0 };
	uint8_t enc_outAddr[ENC_OUTADDR_LEN] = { 0 };
	uint8_t cmd_buf[CMD_LEN + HDW_ACC_BAL_LEN + ENC_OUTADDR_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_TRX_BEGIN;
	cmd_buf[4] = (HDW_ACC_BAL_LEN + ENC_OUTADDR_LEN) & 0xFF;
	cmd_buf[5] = ((HDW_ACC_BAL_LEN + ENC_OUTADDR_LEN) >> 8) & 0xFF;
	cmd_le_dly_rty[0] = BIND_OTP_LEN & 0xFF;
	cmd_le_dly_rty[1] = (BIND_OTP_LEN >> 8) & 0xFF;

	memcpy(outAddr_pad, outAddr, OUTADDR_LEN);
	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	derive_bind_session_key(host_cred, credbuf+1, credbuf + 1 + BIND_OTP_LEN, (uint8_t *)enc_str, strlen(enc_str), bind_senck);
	//dumpdata("bind_senck", bind_senck, BIND_SK_LEN);
	//encrypt mkseed

	hal_aes_buffer_t plain_text = {
	    .buffer = outAddr_pad,
	    .length = sizeof(ENC_OUTADDR_LEN)
	};
	hal_aes_buffer_t key = {
	    .buffer = bind_senck,
	    .length = sizeof(32)
	};
	uint8_t encrypted_buffer[32] = {0};
	hal_aes_buffer_t encrypted_text = {
	    .buffer = enc_outAddr,
	    .length = sizeof(ENC_OUTADDR_LEN)
	};
	hal_aes_ecb_encrypt(&encrypted_text, &plain_text, &key);

	//ikvce_aes_key_initialize(bind_senck, 32, MOP_ECB, ACT_ENC, NULL);
	//ikvce_aes_crypt(enc_outAddr, ENC_OUTADDR_LEN, outAddr_pad, ENC_OUTADDR_LEN / IKVCE_BLOCK_SIZE_AES);

	memcpy(cmd_buf + CMD_LEN, trxAmount, HDW_ACC_BAL_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_BAL_LEN, enc_outAddr, ENC_OUTADDR_LEN);
	memcpy(cmd_buf + CMD_LEN + HDW_ACC_BAL_LEN + ENC_OUTADDR_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + HDW_ACC_BAL_LEN + ENC_OUTADDR_LEN + CMD_LE_DLY_RTY, otp, BIND_OTP_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}
/*
 * [Description  ] HDW prepare transanction verify PIN/OTP/Button
 * [IN Data      ] [OPT/PIN (6 bytes)]
 * [Response data] 
 */
int wallet::hdw_trx_verify(uint8_t IN *rawData, uint8_t IN policy)
{
	hal_sha256_context_t context = {0};
	uint8_t data_len = 0;
	uint8_t verify_data[HAL_SHA256_DIGEST_SIZE] = { 0 };
	uint8_t rspdata[64] = { 0 };
	uint8_t cmd_buf[CMD_LEN + HAL_SHA256_DIGEST_SIZE + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 2bytes(units 1ms), retry 2bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x01, 0x00,
											   0x01, 0x00, };
	if (policy & SECPO_MASK_PIN) {
		data_len = HAL_SHA256_DIGEST_SIZE;
		hal_sha256_init(&context);
		hal_sha256_append(&context, rawData, BIND_OTP_LEN);
		hal_sha256_end(&context, verify_data);
		hal_sha256_init(&context);
		hal_sha256_append(&context, verify_data, HAL_SHA256_DIGEST_SIZE);
		hal_sha256_end(&context, verify_data);
	}
	else {
		data_len = BIND_OTP_LEN;
		memcpy(verify_data, rawData, data_len);
	}
	//clear sensitive data
	memset(rawData, 0, BIND_OTP_LEN);

	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_TRX_VERIFY_OTP;
	cmd_buf[4] = data_len & 0xFF;
	cmd_buf[5] = (data_len >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, verify_data, data_len);
	memcpy(cmd_buf + CMD_LEN + data_len, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + data_len + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	return 0;
}

int wallet::hdw_trx_sign(uint8_t IN inputID, uint8_t OUT *signature)
{
	uint8_t credbuf[64] = { 0 };
	uint8_t mac[MAC_LEN];
	uint8_t rspdata[BCDC_TRX_SIGLEN + MAC_LEN] = { 0 };
	uint8_t cmd_buf[CMD_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
											   0x00, 0x03,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_TRX_SIGN;
	cmd_buf[2] = inputID;

	cmd_le_dly_rty[0] = BCDC_TRX_SIGLEN + MAC_LEN & 0xFF;
	cmd_le_dly_rty[1] = ((BCDC_TRX_SIGLEN + MAC_LEN) >> 8) & 0xFF;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, BCDC_TRX_SIGLEN + MAC_LEN) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}
	dumpData("SIGNATURE", rspdata, BCDC_TRX_SIGLEN);
	
	//check MAC
	if (ReadCred(credbuf) != 0) {
		return -1;
	}
	bind_session_mac(rspdata, BCDC_TRX_SIGLEN, host_cred, credbuf + 1, credbuf + 1 + BIND_OTP_LEN, mac);
	//dumpdata("acc_key_info_mac", mac, MAC_LEN);
	if (memcmp(rspdata + BCDC_TRX_SIGLEN, mac, MAC_LEN) != 0) {
		printf("%s check MAC failed\n", __FUNCTION__);
		return -2;
	}

	memcpy(signature, rspdata, BCDC_TRX_SIGLEN);

	return 0;
}

int wallet::hdw_trx_sign_finish(uint8_t OUT *rspdata)
{
	uint8_t cmd_buf[CMD_LEN + CMD_LE_DLY_RTY] = { 0 };
	//little endian, response length 2bytes, delay time 4bytes(units 1ms), retry 4bytes
	//the highest 2 bits of retry is the scale of delay, 0x4X : delay * 10, 0x8X : delay * 100, 0xCX : delay * 1000
	uint8_t cmd_le_dly_rty[CMD_LE_DLY_RTY] = { 0x00, 0x00,
									 		   0x01, 0x00,
											   0x01, 0x00, };
	cmd_buf[0] = CMD_CLA;
	cmd_buf[1] = INS_TRX_FINISH;

	memcpy(cmd_buf + CMD_LEN, cmd_le_dly_rty, CMD_LE_DLY_RTY);

	if (Kv_Apdu_Transparent(cmd_buf, CMD_LEN + CMD_LE_DLY_RTY, rspdata, 0) != 0) {
		printf("%s failed\n", __FUNCTION__);
		return -1;
	}

	return 0;
}