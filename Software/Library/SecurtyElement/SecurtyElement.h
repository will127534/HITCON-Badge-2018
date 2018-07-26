#include "stdint.h"

#define SPI_MASTER_SYNC     ((uint8_t)0xaa)
#define SPI_SLAVE_SYNC      ((uint8_t)0x55)
#define MAX_RETRY_SYNC        256
#define SYNC_CTRL           ((uint8_t)0x80)
#define SPI_CTRL_DATA     ((uint8_t)0x40)
#define SPI_CTRL_EOT      ((uint8_t)0x20)
#define SPI_FRAME_SIZE      16


#define SPI_STATUS_FINISH   0x01    // Finished the transmit
#define SPI_STATUS_CHKSUM   0x02    
#define SPI_STATUS_BUFFER   0x04    
#define SPI_STATUS_LENGTH   0x08
#define SPI_STATUS_SYNC     0x10
#define SPI_STATUS_NODATA   0x20
/***************************************************************/ 
/***************** MACRO and TYPE definitions ******************/ 
/***************************************************************/
/***************************************************************/ 
/***************** MACRO and TYPE definitions ******************/ 
/***************************************************************/
/* Constants */ 
#define CMD_LEN           6         // CLA INS P1 P2 input_data_len(2B)
#define CMD_BASE_LEN      8         // CMD_LEN(2B) CLA INS P1 P2 input_data_len(2B)
#define RECV_LEN_LEN      2
#define RECV_END_LEN      2

#define KV_STATUS_LEN     2         // Mode length + Last Cmd length
#define KV_VER_LEN        16        // Firmware version length
#define KV_UID_LEN        24        // UID length

/***************************************************************/
/************************ Perso Macro **************************/
/***************************************************************/
#define AUTHMT_LEN        32        // Auth material length
#define SMK_LEN           32        // SMK length 
#define STKN_LEN          4         // Load session token length
#define ENCK_LEN          48        // Key 32 + IV 16
#define MACK_LEN          32
#define MAC_LEN           48
#define HASH_LEN          48
#define COSTKN_LEN        4
#define KVLDR_COS_RECLEN  512
#define BLOCKID_LEN       4
#define BLOCKLEN_LEN      2
/***************************************************************/
/************************ COS Macro **************************/
/***************************************************************/
#define FWVER_LEN         16
#define SN_LEN            24
#define KMTRL_LEN         40
#define ESecstube_LEN     32

#define RAND_LEN          32
#define KEY_LEN           16

#define PARAMETER_LEN     32

#define KRP_LEN           106

/****************************************/
#define CMD_CLA           0x80

/************** preloader ***************/
#define KV_STATUS         0x10
#define KV_VERSION        0x11
#define KV_UID            0x12
#define KV_BACK_IFX       0xCD
#define LOADSESSAUTH      0x20

#define COS_STATUS        0x40
#define COS_LOADBEGIN     0x41
#define COS_LOADFINISH    0x42
#define COS_STREC         0x43
#define COS_WRBLK         0x44
#define COS_JUMP          0xD0
#define COS_RESET         0xF9
#define COS_BACK_LDR        0x78

/****************** COS ******************/
#define ES_GET_FW_VERSION 0x80
#define ES_GET_SN         0x81

#define ES_BUDDY_INIT     0xF0
#define ES_GET_SECSTUB    0xF1
#define ES_SECSTUB_AUTH   0xF2

#define ES_SEC_PARAMTER_WRITE_NWL   0xF4
#define ES_SEC_PARAMTER_READ_NWL    0xF5
#define ES_SEC_PARAMTER_WRITE_WL    0xF7
#define ES_SEC_PARAMTER_READ_WL     0xF8

#define ES_GET_RAND       0xC0
#define ES_SET_KEY        0xC1
#define ES_RESET_KEY      0xC2
#define ES_GEN_KEY        0xC3
#define ES_GET_KEY        0xC4
#define ES_ENCRYPT        0xC5
#define ES_DECRYPT        0xC6

#define ES_VDP_GETCHALLENG  0xE0
#define ES_VDP_AUTH         0xE1
#define ES_VDP_CHANGEKEY    0xE2
#define ES_VDP_WRITEDATA    0xE3
#define ES_VDP_READDATA     0xE4
#define ES_VDP_RESET        0xE5

// test
#define ES_GET_KRP        0xFC

//#define SPI_DEBUG


class Securty_Element{
 public:
  Securty_Element();  // Hardware SPI
  void init(uint8_t ss);
  uint8_t spi_send(uint8_t  tx);
  uint8_t _Chksum(uint8_t *data);
  uint8_t spi_poll_peer_ready (uint8_t ctrl, uint16_t delays);
  uint8_t spi_transmit_frame(uint8_t ctrl, uint8_t *dataToKV, uint8_t *dataFromKV, uint16_t delays);
  int SPI_SND_FRAME(uint16_t length, uint8_t *dataToKV, uint16_t delays);
  int SPI_RCV_FRAME(uint16_t bufferLength, uint8_t *dataFromKV, uint16_t delays);
  void fp_start (void) ;
  void fp_stop(void) ;
  int kill_sle97(int bitNum);

  void dumpData(char* label, uint8_t *data, int len);
  
  uint8_t HexNumConvert(const uint8_t c);
  int loader_cmdhdlr_Echo         (uint8_t *input, uint8_t *output, int len);

  // Info commands
  int loader_cmdhdlr_KvStatus     (uint8_t* status);
  int loader_cmdhdlr_KvVersion    (uint8_t* version);
  int loader_cmdhdlr_KvUid        (uint8_t* uid);
  int loader_cmdhdlr_KvBackSLE97  (void);
  // Perso commands
  int loader_cmdhdlr_LoadsessAuth (uint8_t *authmtrl, uint8_t *encsmk,  uint8_t *stkn, uint8_t *enck, uint8_t *mack );

  // Burn COS commands
  int loader_cmdhdlr_CosStatus    (uint8_t *cosstatus);
  int loader_cmdhdlr_CosLoadBegin (uint8_t cosid, uint8_t *costkn);
  int loader_cmdhdlr_CosLoadFinish(void);
  int loader_cmdhdlr_CosStRec     (uint8_t *costkn, uint8_t recid, uint8_t *rec);
  int loader_cmdhdlr_CosWrBlk     (uint8_t *costkn, uint32_t blkid, uint16_t blklen, uint8_t *hash);
  int loader_cmdhdlr_CosJump      (void);

  // COS commands
  int cos_cmdhdlr_get_fw_version(uint8_t* fwversion);
  int cos_cmdhdlr_get_SN        (uint8_t *SN);

  int cos_cmdhdlr_buddy_init(uint8_t* KMtrl);
  int cos_cmdhdlr_get_secstub(uint8_t Part, uint8_t* ESecstube);
  int cos_cmdhdlr_secstub_auth(uint8_t *response1, uint8_t *challenge2, uint8_t *response2, uint8_t *encSK);
  int cos_cmdhdlr_sec_param_write_nwl(uint8_t BDY_SIDH, uint8_t SIDL, uint8_t *writedata);
  int cos_cmdhdlr_sec_param_read_nwl(uint8_t BDY_SIDH, uint8_t SIDL, uint8_t *readdata);
  int cos_cmdhdlr_sec_param_write_wl(uint8_t ParamID, uint8_t *writedata);
  int cos_cmdhdlr_sec_param_read_wl(uint8_t BDY, uint8_t SID, uint8_t *readdata);
  int cos_cmdhdlr_get_rand(uint8_t *rand);
  int cos_cmdhdlr_set_key(uint8_t keyID, uint8_t *key);
  int cos_cmdhdlr_reset_key(uint8_t keyID);
  int cos_cmdhdlr_gen_key(uint8_t keyID);
  int cos_cmdhdlr_get_key(uint8_t keyID, uint8_t *key);
  int cos_cmdhdlr_encrypt(uint8_t keyID, uint16_t dataLen, uint8_t *inputData, uint8_t *outputData);
  int cos_cmdhdlr_decrypt(uint8_t keyID, uint16_t dataLen, uint8_t *inputData, uint8_t *outputData);
  int cos_cmdhdlr_vdp_getchallenge  (uint8_t *challenge);
  int cos_cmdhdlr_vdp_auth (uint8_t *response1, uint8_t *challenge2, uint8_t *response2, uint8_t *encVDPsk);
  int cos_cmdhdlr_vdp_changekey (uint8_t *encVDPSK);
  int cos_cmdhdlr_vdp_writedata (uint8_t *encVDPData);
  int cos_cmdhdlr_vdp_readdata (uint8_t *encVDPData);
  int cos_cmdhdlr_vdp_reset (uint8_t keyID);
  int cos_cmdhdlr_get_krp(uint8_t *krp);
  int test_get_cos_data(uint8_t *cosdata);
  int cos_cmdhdlr_CosReset(void);
  int cos_cmdhdlr_BackLDR(void);


  int Kv_Apdu_Transparent(uint8_t *data_in, uint16_t size_in, uint8_t *data_out, uint16_t size_out);

  int apdu_send(uint8_t *apdu, uint16_t apdu_len, uint8_t *apdursp, uint16_t lr, uint16_t delay, uint16_t retry16b);

  int error_check(int ret);

 private:
  uint8_t _ss, _clk, _mosi, _miso;
};

