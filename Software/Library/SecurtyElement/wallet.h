#ifndef __Wallet_app_H__
#define __Wallet_app_H__
#include <SecurtyElement.h>
#include "stdint.h"

//enable ethereum 32 bytes amount
#define EN_ETH_BALANCE

//wallet HOST information
// Host credential length (in bytes)
#define HST_CRED_LEN        32

// Host description length (in bytes)
#define HST_DESC_LEN        64

// CLA INS P1 P2 input_data_len(2B)
#define CMD_LEN             6
#define CMD_LE_DLY_RTY      6

#define CMD_CLA             0x80

/* SE Information commands */
#define INS_GET_MODE_STATE         0x10
#define INS_GET_FW_VERSION         0x11

/* Authentication commands */
#define INS_PIN_CHLNG              0x20 
#define INS_PIN_AUTH               0x21 
#define INS_PIN_CHANGE             0x22 
#define INS_PIN_LOGOUT             0x23 
#define INS_PUK_CHLNG              0x24 
#define INS_PIN_UNLOCK             0x25 

/* Personalizaton commands */
#define INS_PERSO_SET_DATA         0x30
#define INS_PERSO_GET_DATA_HASH    0x31
#define INS_PERSO_CONFIRM          0x32
#define INS_PERSO_BACK_PERSO       0x33
#define INS_PERSO_SET_PIN_N        0x34


#define INS_GET_CARD_NAME          0x42 
#define INS_SET_CARD_NAME          0x43 
#define INS_GET_SECPO              0x44 
#define INS_SET_SECPO              0x45 
#define INS_GET_CARDID             0x46 

/* TRX signing commands */
#define INS_TRX_STATUS             0x80
#define INS_TRX_BEGIN              0x72
#define INS_TRX_VERIFY_OTP         0x73
#define INS_TRX_SIGN               0x74
#define INS_TRX_GET_CTXINFO        0x75
#define INS_TRX_FINISH             0x76
#define INS_TRX_OUTADDR            0x79

/* Initialization commands */
#define INS_INIT_SET_DATA          0xA0 
#define INS_INIT_GET_DATA_HASH     0xA1 
#define INS_INIT_CONFIRM           0xA2 
#define INS_INIT_VMK_CHLNG         0xA3 
#define INS_INIT_BACK_INIT         0xA4 
#define INS_INIT_CHANGE_VMK        0xA5 

/* HDW commands */
#define INS_HDW_INIT_WALLET        0xB0
#define INS_HDW_INIT_WALLET_GEN    0xB1
#define INS_HDW_QRY_WA_INFO        0xB2
#define INS_HDW_SET_WA_INFO        0xB3
#define INS_HDW_CREATE_ACCOUNT     0xB4
#define INS_HDW_QRY_ACC_INFO       0xB5
#define INS_HDW_SET_ACC_INFO       0xB6
#define INS_HDW_NEXT_TRX_ADDR      0xB7
#define INS_HDW_PREP_TRX_SIGN      0xB8
#define INS_HDW_INIT_WA_GEN_CONF   0xB9
#define INS_HDW_QRY_ACC_KEYINFO    0xBA
#define INS_HDW_GEN_ENTROPY        0xBB
#define INS_HDW_MNEMONIC_TO_SEED   0xBC
#define INS_HDW_INIT_WALLET_WORDS  0xBD

/* Host Binding commands */
#define INS_BIND_REG_INIT          0xD0
#define INS_BIND_REG_CHLNG         0xD1
#define INS_BIND_REG_FINISH        0xD2
#define INS_BIND_REG_INFO          0xD3
#define INS_BIND_REG_APPROVE       0xD4
#define INS_BIND_REG_REMOVE        0xD5
#define INS_BIND_LOGIN_CHLNG       0xD6
#define INS_BIND_LOGIN             0xD7
#define INS_BIND_LOGOUT            0xD8
#define INS_BIND_FIND_HSTID        0xD9
#define INS_BIND_BACK_NOHOST       0xDA


/* init data list */
#define BCDC_IDATA_UPINHASH 0    /* User PIN Hash */ 
#define BCDC_IDATA_PUK      1    /* PUK (PIN Unlock Key) */ 
#define BCDC_IDATA_CARDID   3    /* Card ID */ 

/* VMK length (in bytes) */
#define VMK_LEN             32 

/* Challenge/response length (in bytes) */
#define AUTHUTIL_CHLNG_LEN  16 
#define AUTHUTIL_RESP_LEN   16 

#define CARD_ID_LEN         8

#define HANDLE_LEN          4

// OTP key length (in bytes)
#define OTPKEY_LEN          32

// Binding session key length (in bytes)
#define BIND_SK_LEN         32

// Number of binding OTP digits
#define BIND_OTP_LEN        6

/* MAC value length (in bytes) */
#define MAC_LEN             32 

/* Length of binding challenge and response */
#define BIND_CHLNG_LEN      16
#define BIND_RESP_LEN       16
#define PIN_RESP_LEN        16

/* PIN hash length (in bytes) */
#define PIN_LEN             6
#define PINHASH_LEN         32 

#define BIND_FIN_RSP_LEN    2

#define BIND_INIT_BUF_LEN         CMD_LEN + HST_CRED_LEN + HST_DESC_LEN + HAL_SHA256_DIGEST_SIZE + CMD_LE_DLY_RTY
#define BIND_CHLNG_BUF_LEN        CMD_LEN + HANDLE_LEN + CMD_LE_DLY_RTY
#define BIND_FINISH_BUF_LEN       CMD_LEN + HANDLE_LEN + BIND_RESP_LEN + PIN_RESP_LEN + CMD_LE_DLY_RTY

//store RegOTP and Login Challeng
#define BIND_CRED_FILE_NAME      "bind_host.bin"
#define BIND_CRED_FILE_HEADER    "HostID OTP_KEY:"

/* bcdc_info module storage length (in bytes) */
#define BCDC_INFO_STORE_LEN        60

/* Card name length (in bytes) */
#define BCDC_CARDNAME_LEN          32 
/* Currency length (in bytes) */
#define BCDC_CURRENCY_LEN          5 
/* Security policy length (in bytes) */
#define BCDC_SECPO_LEN             4 
/* Card ID length (in bytes) */
#define BCDC_CARDID_LEN            8 

/* BCDC info data length */
#define BCDC_INFO_CARDNAME_LEN  (BCDC_CARDNAME_LEN)    /* Card name */
#define BCDC_INFO_CURRENCY_LEN  (BCDC_CURRENCY_LEN)    /* Currecy */
#define BCDC_INFO_SECPO_LEN     (BCDC_SECPO_LEN)       /* Security policy */
#define BCDC_INFO_CARDID_LEN    (BCDC_CARDID_LEN)      /* Card ID */


/* Length of HD wallet name (in bytes) */
#define HDW_NAME_LEN               32

/* BIP39 generated seed length (in bytes) */
#define BIP39_SEED_LEN             64
#define PASSPHRASE_MAX_LEN         16
#define MNEMONIC_LEN               240                 // > (8 + 1) * 24 = 216 , (english 3-8 letters plus space) * (maximum 24 words)

#define INIT_WALLET_BUF_LEN        CMD_LEN + HDW_NAME_LEN + BIP39_SEED_LEN + MAC_LEN + CMD_LE_DLY_RTY
#define INIT_WALLET_MNE_BUF_LEN    CMD_LEN + HDW_NAME_LEN + PASSPHRASE_MAX_LEN + MNEMONIC_LEN + MAC_LEN + CMD_LE_DLY_RTY

/* HDW info */
#define HDW_INFO_STATE             0    /* HDW status (1 byte) */
#define HDW_INFO_NAME              1    /* HDW name (32 bytes) */
#define HDW_INFO_ACCPTR            2    /* Account pointer (4 bytes) */
#define HDW_INFO_NUM               3

/* HDW account name length (in bytes) */
#define HDW_ACCNAME_LEN            32
#define HDW_ACC_PURPOSE_LEN        4
#define HDW_ACC_COINTYPE_LEN       4
#define HDW_ACC_ID_LEN             4
#ifdef EN_ETH_BALANCE
#define HDW_ACC_BAL_LEN            32
#else
#define HDW_ACC_BAL_LEN            8
#endif
#define HDW_ACC_KPTR_LEN           4    //EXTKPTR or INTKPTR
#define HDW_ACC_INFO_ALL_LEN       HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_BAL_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_KPTR_LEN + HDW_ACC_BAL_LEN
#define HDW_ACC_INFO_CT_OFST       HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN
#define HDW_ACC_INFO_ID_OFST       HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN
#define HDW_ACC_INFO_EPTR_OFST     HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_BAL_LEN
#define HDW_ACC_INFO_IPTR_OFST     HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_BAL_LEN + HDW_ACC_KPTR_LEN


/* HDW account info ID */
#define HDW_ACCINFO_NAME           0   /* Account name (32 bytes) */
#define HDW_ACCINFO_PURPOSE        1   /* Account purpose (4 bytes) */
#define HDW_ACCINFO_COINTYPE       2   /* Account coin type (4 bytes) */
#define HDW_ACCINFO_BALNC          3   /* Account balance (8 bytes) */
#define HDW_ACCINFO_EXTKPTR        4   /* External key pointer (4 bytes) */
#define HDW_ACCINFO_INTKPTR        5   /* Internal key pointer (4 bytes) */
#define HDW_ACCINFO_XCHSBLK        6   /* XCHS blocked balance (8 bytes) */
#define HDW_ACCINFO_NUM            7   /* Number of account info */

#define HDW_CREATE_ACC_BUF_LEN     CMD_LEN + HDW_ACCNAME_LEN + HDW_ACC_PURPOSE_LEN + HDW_ACC_COINTYPE_LEN + HDW_ACC_ID_LEN + CMD_LE_DLY_RTY

/* HDW account key info ID */
#define HDW_ACCKINFO_ADDR          0   /* Key address (25 bytes) */
#define HDW_ACCKINFO_KPUB          1   /* Public key  (64 bytes) */
#define HDW_ACCKINFO_ACCKPUB       2   /* Account public key  (64 bytes) */

/* Bitcoin binary address length (in bytes) */
#define BC_BINADDR_LEN             25
/* Public key length (in bytes) */
#define HDW_KPUB_LEN               64
/* Chain code length (in bytes) */
#define HDW_CHCODE_LEN             32
#define HDW_ACCKPUB_LEN            HDW_KPUB_LEN + HDW_CHCODE_LEN

/* Trx output address length */
#define ENC_OUTADDR_LEN            48    /* Encrypted */
#define OUTADDR_LEN                34    /* Plaintext */

#define BCDC_TRX_SIGLEN            65   /* r, s, v*/ 
#define SECPO_MASK_OTP      (1 << 0) 
#define SECPO_MASK_BUTTON   (1 << 1) 
#define SECPO_MASK_PIN      (1 << 2) 
#define SECPO_MASK_PINLOCK  (1 << 3) 
class wallet: public Securty_Element{
 public:
wallet();
//Info
int GetModeState(uint8_t *respdata);

uint8_t host_cred[HST_CRED_LEN] = { 0 };
uint8_t host_desc[HST_DESC_LEN] = { 0 };
uint8_t _Cred_buf[1 + BIND_OTP_LEN + BIND_CHLNG_LEN] = { 0 };


int WriteCred(uint8_t *Cred_buf);
int ReadCred(uint8_t *Cred_buf);

int SetInitData(uint8_t ID, uint8_t preHostID, uint8_t *initData, uint8_t rawlen);
int GetInitDataHash(uint8_t ID, uint8_t preHostID, uint8_t *Hash);
int InitVMK_CHLNG(uint8_t *chlng);
int InitChangeVMK(uint8_t *vmkChlng,uint8_t *curVMK, uint8_t *newVMK);
int PIN_CHLNG(bool PIN_AUTH, uint8_t *chlng);
int ModeAuth(uint8_t *inputPIN, uint8_t *pinChlng);
int Change_PIN(uint8_t *oldPIN, uint8_t *newPIN);
int InitBackInit(uint8_t *vmkChlng, uint8_t *curVMK);
int InitDataConfirm(void);
int SetCardName(uint8_t *Name);
int GetCardName(uint8_t *rspdata);

int GetSECPO(uint8_t *SECPO);
int SetSECPO(uint8_t *SECPO, uint8_t *inputPIN, uint8_t *pinChlng);
int perso_set_pin_n(uint8_t *PIN);
//Host binding
int BindRegInit(uint8_t firstHost, uint8_t *HostCred, uint8_t *HostDesc, uint8_t *rspdata);
int BindRegChallenge(uint8_t *brHandle, uint8_t *RegChlng);
int BindRegFinish(uint8_t *brHandle, uint8_t *HostCred, uint8_t *RegOtp, uint8_t *RegChlng, uint8_t *rspdata);
int BindRegInfo(uint8_t hostID, uint8_t *RegInfo);

//Host login
int BindLoginChallenge(uint8_t hostID, uint8_t *LoginChlng);
int BindLogin(uint8_t hostID, uint8_t *HostCred, uint8_t *RegOtp, uint8_t *LoginChlng, uint8_t *rspdata);
int BindLogout(void);
int PIN_CHLNG(uint8_t *chlng);
int BindBackToNoHost(uint8_t *pinChlng, uint8_t *pinHash);

//Perso setting
int perso_set_data(uint8_t *HostCred, uint8_t *RegOtp, uint8_t *LoginChlng, uint8_t *rspdata);
int perso_confirm(uint8_t *rspdata);

//hdw func
int hdw_init_wallet(uint8_t *hdwName, uint8_t *rspdata);
int hdw_init_wallet_words(uint8_t *hdwName, char *passphrase, uint8_t passphr_len, char *mnemonic, uint8_t mnemonic_len, uint8_t *rspdata);
int hdw_qry_wa_info(uint8_t *rspdata);

int hdw_create_account(uint8_t *accName, uint8_t *accPurpose, uint8_t *accCoinType, uint8_t *accID, uint8_t *rspdata);
int hdw_qry_acc_info(uint8_t *accID, uint8_t *rspdata);
int hdw_set_acc_info(uint8_t InfoID, uint8_t *accID, uint8_t *accInfo, uint8_t *rspdata);

int hdw_qry_acc_key_info(uint8_t InfoID, uint8_t KCID, uint8_t *accID, uint8_t *keyID, uint8_t *rspdata);
int hdw_gen_entropy(uint16_t entropy_bit_len, uint8_t *rspdata);
int hdw_mnemonic_to_seed(char *passphrase, char *mnemoinc, uint8_t *rspdata);

int hdw_next_trx_addr(uint8_t KCID, uint8_t *accID, uint8_t *rspdata);

int hdw_prep_trx_sign(uint8_t inputID, uint8_t kcID, uint8_t *accID, uint8_t *keyID, uint8_t *balnc, uint8_t *trxHash, uint8_t *rspdata);
int hdw_trx_sign_begin(uint8_t *trxAmount, uint8_t *outAddr, uint8_t *otp);
int hdw_trx_verify(uint8_t *rawData, uint8_t policy);
int hdw_trx_sign(uint8_t inputID, uint8_t *signature);
int hdw_trx_sign_finish(uint8_t *rspdata);

 
int derive_bind_session_key(uint8_t *HostCred, uint8_t *RegOtp, uint8_t *LoginChlng,uint8_t *mtrl, uint32_t mtrl_len, uint8_t *sess_key);
int bind_session_mac(uint8_t *data, uint32_t data_len, uint8_t *HostCred,uint8_t *RegOtp, uint8_t *LoginChlng, uint8_t *mac);
};



#endif
