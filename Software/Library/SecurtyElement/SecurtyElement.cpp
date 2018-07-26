#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <SPI.h>
#include "SecurtyElement.h"
#include "stdint.h"
//#define SPI_DEBUG
#define setbit(reg, bit, mask, val) (((reg) & ~((mask) << (bit))) | ((val) << (bit)))
char* error_names[] = {"","Command not supported","Wrong mode ID","Wrong APDU LC","Wrong test function ID","Wrong trx signing state","Trx OTP verification fail","Wallet is not active","Wallet is active","Wallet id mismatch","Wrong output id for trx_sign","Waiting for cdata timeout (7816 interface only)","No response data (7816 interface only)","Fail to pass hash check","Fail to pass wallet address check","Wrong BCDC init state","Wrong input init data information","Wrong init data state","Wrong BCDC perso state","Wrong input perso data information","Wrong perso data state","DRNG module failed to generate random bytes","Wrong input ID","No auth challenge generated","Auth locked","Auth fail, not locked yet","Auth fail and locked","Not authed yet","Not locked","Wrong test sub-function ID","No card name exists","Wrong wallet ID","Wrong export wallet info ID","","Wrong trx context INFO ID","Wrong wallet package ID","Internal module error","Wrong backup status","Wrong backup handle","Wrong wallet status","Wrong restore status","Wrong restore checksum","Wrong restore handle","Wrong restore seed ID","Wrong binding host ID","Not in binding login state","Wrong host binding status","Wrong host login status","Binding login fail","Wrong HD wallet status","Wrong number set length","Wrong HDW info ID","Wrong HDW info length","Wrong HDW account purpose","Wrong HDW account coin type","Wrong HDW account ID","Wrong HDW account info ID","Wrong key chain ID","Wrong key ID","Wrong account info length","Wrong activation code","Wrong account pointer value","Out of keys","Already no host","Wrong first flag","Full of hosts","Wrong host registration status","Wrong brhandle","Wrong registration response","Back IKV loader auth fail","Fail to back to IKV loader","","","","","","","","Wrong credential handle","Wrong number set checksum","Wrong MAC value","Wrong pre-reg host ID","Fail to perform NVM read","Fail to perform NVM write","Wrong trx amount","Wrong trx signature type","Wrong length or content of mnemonic","Wrong length of BIP39 passphras"};


static uint8_t send_data[1024], recv_data[1024];
int bufferLen = 1024;
uint16_t frame_delays = 100;
static uint8_t spi_status;
int err_ret=0;

Securty_Element::Securty_Element()
{

}
  
void Securty_Element::init(uint8_t ss)
{
  _ss = ss;

  pinMode(_ss, OUTPUT);
  digitalWrite(_ss, HIGH);
  SPI.begin();
  SPI.beginTransaction(SPISettings(14000000, MSBFIRST, SPI_MODE1));
}



uint8_t Securty_Element::spi_send(uint8_t  tx) 
{
  uint8_t ret;
  ret = SPI.transfer(tx);
  return ret;
}

void Securty_Element::fp_start (void) 
{ 
  digitalWrite(_ss, LOW);
}

void Securty_Element::fp_stop(void) 
{ 
  digitalWrite(_ss, HIGH);
}


uint8_t Securty_Element::spi_poll_peer_ready (uint8_t ctrl, uint16_t delays)
{
  int cnt = 0;
  uint16_t tx;
  uint16_t rx;

  /* 1. SYNC */
  /* 2. CONTROL */
  /* 3. RETURN */
  
  /* SYNC 0xAA <--> 0x55 */
  tx = SPI_MASTER_SYNC;
  while(cnt++ < MAX_RETRY_SYNC) {
    rx = spi_send(tx);
    if(rx == SPI_SLAVE_SYNC) {      
      
      delayMicroseconds(delays * 50);
      break;
    } else {
      //Serial.print("cnt = ");Serial.print(cnt);Serial.print(" ,rx = ");Serial.println(rx,HEX);
      delayMicroseconds(delays * 50);
    }
  }
  if(cnt >= MAX_RETRY_SYNC) {
    return 0;
  }
  // delay between sync and ctrl
  delayMicroseconds(delays * 50);
  
  
  #ifdef SPI_DEBUG
    printf("SYNK tx = %04x, rx = %04x\r\n", tx, rx);
  #endif
  /* CTRL */
  tx = ctrl & (~SYNC_CTRL);
  while(cnt++ < MAX_RETRY_SYNC) {
    rx = spi_send(tx);
    if (rx & SYNC_CTRL) {
      break;
    }
    delayMicroseconds(delays * 50);
  }
  delayMicroseconds(delays * 50);
  //printf("cnt = %d, rx = %02X\r\n", cnt, rx);
  #ifdef SPI_DEBUG
    printf("CTRL tx = %04x, rx = %04x\r\n", tx, rx);
  #endif  
  /* RETURN */
  if(cnt >= MAX_RETRY_SYNC) {
    return 0;
  } else {
    // slave control
    return rx;
  }
}

uint8_t Securty_Element::spi_transmit_frame(uint8_t ctrl, uint8_t *dataToKV, uint8_t *dataFromKV, uint16_t delays)
{
  int i;
  uint8_t slave_ctrl;
  uint8_t master_chksum = 0, slave_chksum = 0, slave_chksum_fromKV;
  /* SPI Transmit */
  /* 0. ES4_KV1_UTIL_START_TRANSMIT */
  /* 1. spi_poll_peer_ready */
  /* 2. Transmit Frame Data Part (16 bytes) */
  /* 3. Transmit Frame Checksum  ( 1 bytes) */
  delayMicroseconds(500);
  fp_start();
  slave_ctrl = spi_poll_peer_ready(ctrl, delays);
  if(slave_ctrl == 0) {
    for(i = 0; i < 256; i++) {
      spi_send(0x00);
      delayMicroseconds(200);
    }
    fp_stop();
    return 0;
  }
  
  /* 2. Transmit Frame Data Part (16 bytes) */
  for (i = 0; i < SPI_FRAME_SIZE; i++) {
   
    dataFromKV[i] =  spi_send(dataToKV[i]);
    
    master_chksum = (master_chksum + dataToKV[i]) & 0xFF;
    slave_chksum  = (slave_chksum  + dataFromKV[i]) & 0xFF;
    delayMicroseconds(10);
  }
  
  
  /* 3. Transmit Frame Checksum  ( 1 bytes) */
  master_chksum = (~master_chksum + 1) & 0xFF;
  slave_chksum  = (~slave_chksum + 1) & 0xFF;
  
  slave_chksum_fromKV = spi_send(master_chksum);
  fp_stop();
  //delayMicroseconds(delays*10000);

  #ifdef SPI_DEBUG
    //printf("==> SLE97 (CTRL = [%02X]): ", ctrl);
    Serial.println("*****************************************************************************\r\n");
  
    Serial.print("==> SLE97 (CTRL = [");Serial.print(ctrl,HEX);Serial.print("]): ");
    for(i = 0; i < SPI_FRAME_SIZE; i++) {
      if(dataToKV[i]<16){
        Serial.print("0");
      }
      Serial.print(dataToKV[i],HEX);Serial.print(" ");
    }
    Serial.print("[");Serial.print(master_chksum,HEX);Serial.println("] ");
    //printf("[%02X]\r\n", master_chksum);
    Serial.print("<== SLE97 (CTRL = [");Serial.print(slave_ctrl,HEX);Serial.print("]): ");
    //printf("<== SLE97 (CTRL = [%02X]): ", slave_ctrl);
    for(i = 0; i < SPI_FRAME_SIZE; i++) {
      if(dataFromKV[i]<16){
        Serial.print("0");
      }
      Serial.print(dataFromKV[i],HEX);Serial.print(" ");
    }
    Serial.print("[");Serial.print(slave_chksum_fromKV,HEX);Serial.println("] ");
    //printf("[%02X]\r\n", slave_chksum_fromKV);
    Serial.println("*****************************************************************************\r\n");
  #endif

  if(slave_chksum != slave_chksum_fromKV) {
    spi_status |= SPI_STATUS_CHKSUM;
  }
  return slave_ctrl;
}

int Securty_Element::SPI_SND_FRAME(uint16_t length, uint8_t *dataToKV, uint16_t delays) {
  int i;
  uint8_t slave_ctrl;
  uint8_t frame_buf[SPI_FRAME_SIZE];
  spi_status = 0;
  /* Transmit a frame */
  for(i = 0; i < length; i += SPI_FRAME_SIZE) {
    
    if(length <= i + SPI_FRAME_SIZE) {
      slave_ctrl = spi_transmit_frame(SPI_CTRL_DATA | SPI_CTRL_EOT, dataToKV + i, frame_buf, delays);
      if(slave_ctrl == 0) {
        spi_status |= SPI_STATUS_SYNC;
        return spi_status;
      }
    } else {
      slave_ctrl = spi_transmit_frame(SPI_CTRL_DATA & ~SPI_CTRL_EOT, dataToKV + i, frame_buf, delays);
      if(slave_ctrl == 0) {
        spi_status |= SPI_STATUS_SYNC;
        return spi_status;
      } 
    }




    if (slave_ctrl != 0xA0) {
        Serial.print("!!CTRL = ");Serial.print(slave_ctrl,HEX);Serial.println("==>");
        for(i = 0; i < SPI_FRAME_SIZE; i++) {
            Serial.print(frame_buf[i],HEX);
        }
        Serial.println("");
    }
  }
  return spi_status | SPI_STATUS_FINISH;  
}

int Securty_Element::SPI_RCV_FRAME(uint16_t bufferLength, uint8_t *dataFromKV, uint16_t delays)
{
  int i;
  uint8_t slave_ctrl;
  uint8_t frame_buf[SPI_FRAME_SIZE] = {0};
  uint16_t packet_length_FromKV = 0;
  
  // spi_apdu_sw[0] = 0x00;
  // spi_apdu_sw[1] = 0x00;
  spi_status = 0;
  
  if(bufferLength < SPI_FRAME_SIZE) {
    spi_status |= SPI_STATUS_BUFFER;
    return spi_status;
  }
  
  /* Transmit First Frame (Get Length) */
  slave_ctrl = spi_transmit_frame(SPI_CTRL_EOT, frame_buf, dataFromKV, delays);
  if(slave_ctrl & SPI_CTRL_DATA) {
    packet_length_FromKV = dataFromKV[0] + (dataFromKV[1] << 8) + 2;
    /*printf("CTRL = %02X ==> ", slave_ctrl);
    for(i = 0; i < SPI_FRAME_SIZE; i++) {
      printf("%02X ", dataFromKV[i]);
    }
    printf("\r\n");*/
  } else if (slave_ctrl == 0) {
    spi_status |= SPI_STATUS_SYNC;
    return spi_status;
  } else {
    /*printf("CTRL = %02X ==> ", slave_ctrl);
    for(i = 0; i < SPI_FRAME_SIZE; i++) {
      printf("%02X ", dataFromKV[i]);
    }
    printf("\r\n");*/
    spi_status |= SPI_STATUS_NODATA;
    return spi_status;
  }
  
  if(bufferLength < packet_length_FromKV) {
    spi_status |= SPI_STATUS_BUFFER;
  }
  /* Transmit a frame */
  for(i = SPI_FRAME_SIZE; i < bufferLength && i < packet_length_FromKV && ((slave_ctrl & SPI_CTRL_EOT) == 0); i += SPI_FRAME_SIZE) {
    slave_ctrl = spi_transmit_frame(SPI_CTRL_EOT, frame_buf, dataFromKV + i, delays);
    if(!(slave_ctrl & SPI_CTRL_DATA)) {
      Serial.print("Transmit a frame : spi_status |= SPI_STATUS_NODATA\r\n");
      spi_status |= SPI_STATUS_NODATA;
      return spi_status;
    } else if (slave_ctrl == 0) {
      spi_status |= SPI_STATUS_SYNC;
      return spi_status;
    }
  }
  
  if(i < packet_length_FromKV)
  {
    spi_status |= SPI_STATUS_LENGTH;
  } 
  return spi_status | SPI_STATUS_FINISH;
}



uint8_t Securty_Element::HexNumConvert(const uint8_t c) {
    if (c >= '0' && c <= '9') {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    }
    else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    }
    else {
        return (uint8_t)-1;
    }
}


/*
 * send_data:
 *
 *                 2B       2B    1B   1B          2B
 *            | send_len | cmd  | P1 | P2 |  input_data_len | input_data
 *
 *
 * recv_data:
 *                 2B                                        2B
 *            | recv_len |         recv_data          |  recv_status 
 *
 */

int Securty_Element::error_check(int ret)
{
  switch (ret){
    case SPI_STATUS_FINISH :
      return 0;
    case SPI_STATUS_BUFFER:
      //printf("Recv Error at bufferLength\r\n");
      return -1;
    case SPI_STATUS_SYNC :
      //printf("Recv Error at SYNC\r\n");
      return -1;
    case SPI_STATUS_NODATA :
      //printf("Recv Error at NODATA\r\n");
      return -1;
    case SPI_STATUS_LENGTH | SPI_STATUS_FINISH :
      //printf("Recv Error at recv_len length\r\n");
      return -1;
    default:
      //printf("Error\r\n");
      return -1;
  }
}
  
/********************* Preloader commands ***********************/

/* Echo (unstable)
 * cmd: 0x80 0xBE
 * In:  input (len B)
 * Out: output (len B)
 */

int Securty_Element::loader_cmdhdlr_Echo(uint8_t *input, uint8_t *output, int len) {
  uint8_t send_data[SPI_FRAME_SIZE + 16] = { 0x00, 0x00, 0x80, 0xBE, 0x00, 0x00, 0x00, 0x00 };
  uint8_t recv_data[SPI_FRAME_SIZE];
    
  send_data[0] = (len + 8) & 0xFF;
  send_data[1] = (len + 8) >> 8;
  send_data[6] = len & 0xFF ;       
  send_data[7] = len >> 8;        
  
  memcpy(send_data + 8, input, len);
  
  SPI_SND_FRAME(len + 8, send_data, 100);
  delayMicroseconds(100);
  
  if(SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays) != 1) {
    return 1;
  }
  memcpy(output, recv_data + 2, len);
  return 0;
}


/* Info commands */


/* Get KvStatus
 * cmd: 0x80 0x10
 * Out: Mode(1B), Last Cmd(1B)
 */
int Securty_Element::loader_cmdhdlr_KvStatus(uint8_t* status) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = KV_STATUS;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(100);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
    
  memcpy(status, recv_data + 2, KV_STATUS_LEN);
  return 0;
}

/* Get KvVersion
 * cmd: 0x80 0x11
 * Out: Version (16B)
 */
int Securty_Element::loader_cmdhdlr_KvVersion(uint8_t* version) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = KV_VERSION;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(100);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + KV_VER_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    //Serial.println(err_ret);
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + KV_VER_LEN + RECV_END_LEN);
    return -1;
  }
  if (recv_data[RECV_LEN_LEN + 16] != 0x90 || recv_data[RECV_LEN_LEN + 16 + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 16 + RECV_END_LEN);
    return -1;
  }
  
  memcpy(version, recv_data + RECV_LEN_LEN, KV_VER_LEN);
  return 0;
}

/* Get KvUid
 * cmd: 0x80 0x12
 * Out: uid (24B)
 */
int Securty_Element::loader_cmdhdlr_KvUid(uint8_t* uid) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = KV_UID;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(100);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + KV_UID_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + KV_UID_LEN + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 24] != 0x90 || recv_data[RECV_LEN_LEN + 24 + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 24 + RECV_END_LEN);
    return -1;
  }
  memcpy(uid, recv_data + RECV_LEN_LEN, KV_UID_LEN);
  return 0;
}


/************************ Perso commands ************************/

/* LoadsessAuth
 * cmd: 0x80 0x20
 * In:  AuthMtrl (32B), EncSMK(32B)
 * Out: SessionToken(4B), EncKey+IV(48B), MacKey(32B)
 */
int Securty_Element::loader_cmdhdlr_LoadsessAuth(uint8_t *authmtrl, uint8_t *encsmk,  uint8_t *stkn, uint8_t *enck, uint8_t *mack ) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;
  send_data[0] = CMD_LEN+ AUTHMT_LEN + SMK_LEN;   // total length except first two byte
  send_data[2] = CMD_CLA;
  send_data[3] = LOADSESSAUTH;
  send_data[6] = AUTHMT_LEN + SMK_LEN;            // input data length
  
  memcpy(send_data + CMD_BASE_LEN, authmtrl, AUTHMT_LEN);
  memcpy(send_data + CMD_BASE_LEN + AUTHMT_LEN, encsmk, SMK_LEN);
  
  SPI_SND_FRAME(CMD_BASE_LEN + AUTHMT_LEN + SMK_LEN, send_data, frame_delays);
  //delayMicroseconds(100);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + STKN_LEN + ENCK_LEN + MACK_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN + AUTHMT_LEN + SMK_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + STKN_LEN + ENCK_LEN + MACK_LEN + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + STKN_LEN + ENCK_LEN + MACK_LEN] != 0x90 || recv_data[RECV_LEN_LEN + STKN_LEN + ENCK_LEN + MACK_LEN + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + STKN_LEN + ENCK_LEN + MACK_LEN + RECV_END_LEN);
    return -1;
  }
  
  memcpy(stkn, recv_data + RECV_LEN_LEN, STKN_LEN);
  memcpy(enck, recv_data + RECV_LEN_LEN + STKN_LEN, ENCK_LEN);
  memcpy(mack, recv_data + RECV_LEN_LEN + STKN_LEN + ENCK_LEN, MACK_LEN);
  return 0;
}

/* Get Cos Status
 * cmd: 0x80 0x40
 * Out: CosStatus (1B)
 */
int Securty_Element::loader_cmdhdlr_CosStatus(uint8_t *cosstatus) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = COS_STATUS;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 1] != 0x90 || recv_data[RECV_LEN_LEN + 1 + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 1 + RECV_END_LEN);
    return -1;
  }
  memcpy(cosstatus, recv_data + 2, 1);
  return 0;
}

/* Cos Load Begin
 * cmd: 0x80 0x41
 * In : CosId(1B)
 * Out: CosToken(4B)
 */
int Securty_Element::loader_cmdhdlr_CosLoadBegin(uint8_t cosid, uint8_t *costkn) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  ////Serial.print("loader_cmdhdlr_CosLoadBegin\r\n");
  send_data[0] = CMD_LEN + 1;
  send_data[2] = CMD_CLA;
  send_data[3] = COS_LOADBEGIN;
  send_data[8] = cosid;
  int err_ret=0;
  SPI_SND_FRAME(CMD_BASE_LEN + 1, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN + 1);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 4] != 0x90 || recv_data[RECV_LEN_LEN + 4 + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 4 + RECV_END_LEN);
    return -1;
  }
  memcpy(costkn, recv_data + RECV_LEN_LEN, COSTKN_LEN);
  return 0;
}

/* Cos Load Finish
 * cmd: 0x80 0x42 
 */
int Securty_Element::loader_cmdhdlr_CosLoadFinish(void) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = COS_LOADFINISH;
  ////Serial.print("loader_cmdhdlr_CosLoadFinish\r\n");
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN );
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  }
  
  return 0;
}

/* Cos Set Rec
 * cmd: 0x80 0x43
 * In : CosToken(4B) RecId(1B) Rec(512B)
 */
int Securty_Element::loader_cmdhdlr_CosStRec(uint8_t *costkn, uint8_t recid, uint8_t *rec) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;

  //Serial.print("loader_cmdhdlr_CosStRec, recid = ");
  //Serial.println(recid);
  send_data[0] = (CMD_LEN + COSTKN_LEN + 1 + KVLDR_COS_RECLEN) & 0xFF;    // total len
  send_data[1] = (CMD_LEN + COSTKN_LEN + 1 + KVLDR_COS_RECLEN) >> 8;      // total len
  send_data[2] = CMD_CLA;
  send_data[3] = COS_STREC;
  send_data[6] = (COSTKN_LEN + 1 + KVLDR_COS_RECLEN) & 0xFF;        // input data len
  send_data[7] = (COSTKN_LEN + 1 + KVLDR_COS_RECLEN) >> 8;          // input data len
  
  memcpy(send_data + CMD_BASE_LEN, costkn, COSTKN_LEN);
  memcpy(send_data + CMD_BASE_LEN + COSTKN_LEN, &recid, 1);
  memcpy(send_data + CMD_BASE_LEN + COSTKN_LEN + 1, rec, KVLDR_COS_RECLEN);
    
  SPI_SND_FRAME(CMD_BASE_LEN + COSTKN_LEN + 1 + KVLDR_COS_RECLEN, send_data, frame_delays);
  delayMicroseconds(5000);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if (error_check(err_ret) != 0) 
  {
    //dumpData("send_data", send_data, CMD_BASE_LEN + COSTKN_LEN + 1 + KVLDR_COS_RECLEN );
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[2] != 0x90 || recv_data[3] != 0x00) {
    dumpData("recv_data", recv_data, 4);
    return -1;
  }
  return 0;
}

/* Cos Write Block
 * cmd: 0x80 0x44
 * In : CosToken(4B) BlockId(4B) BlockLen(2B) Hash(48B)
 */
int Securty_Element::loader_cmdhdlr_CosWrBlk(uint8_t *costkn, uint32_t blkid, uint16_t blklen, uint8_t *hash) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;

  //Serial.print("loader_cmdhdlr_CosWrBlk, blkid = ");
  //Serial.print(blkid);
  //Serial.print(", blklen = ");
  //Serial.println(blklen);
  send_data[0] = CMD_LEN + COSTKN_LEN + BLOCKID_LEN + BLOCKLEN_LEN + HASH_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = COS_WRBLK;
  send_data[6] = COSTKN_LEN + BLOCKID_LEN + BLOCKLEN_LEN + HASH_LEN;  
  
  memcpy(send_data + CMD_BASE_LEN, costkn, COSTKN_LEN);
  memcpy(send_data + CMD_BASE_LEN + COSTKN_LEN, &blkid, BLOCKID_LEN);
  memcpy(send_data + CMD_BASE_LEN + COSTKN_LEN + BLOCKID_LEN, &blklen, BLOCKLEN_LEN);
  memcpy(send_data + CMD_BASE_LEN + COSTKN_LEN + BLOCKID_LEN + BLOCKLEN_LEN, hash, HASH_LEN);
    
  SPI_SND_FRAME(CMD_BASE_LEN + COSTKN_LEN + BLOCKID_LEN + BLOCKLEN_LEN + HASH_LEN, send_data, frame_delays);
  delayMicroseconds(1000);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[2] != 0x90 || recv_data[3] != 0x00) {
    dumpData("recv_data", recv_data, 4);
    return -1;
  }
  return 0;
}

/* KvBackSLE97
 * Management command
 * cmd: 0x80 0xC6
 */
int Securty_Element::loader_cmdhdlr_KvBackSLE97(void) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = KV_BACK_IFX;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(100);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + KV_UID_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + KV_UID_LEN + RECV_END_LEN);
    return 1;
  }
  
  //Serial.print("KvBackSLE97 SUCCESS");
  return 0;
}

/* Cos Jump
 * cmd: 0x80 0xD0 
 */
int Securty_Element::loader_cmdhdlr_CosJump(void) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;

  send_data[0] = 0x0A;
  send_data[2] = CMD_CLA;
  send_data[3] = COS_JUMP;
  send_data[6] = 0x04;  
  
  send_data[8] =  0x19; 
  send_data[9] =  0x95; 
  send_data[10] = 0x19; 
  send_data[11] = 0x96; 

  SPI_SND_FRAME(12, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, 12);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return -1;
  }
  //dumpData("send_data", send_data, 12);
  //dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  }
  return 0;
}

/*********************** COS commands ***********************/

/* get fw version
 * cmd: 0x80 0x80 
 * Out: FW(16B) SW(2B)
*/
int Securty_Element::cos_cmdhdlr_get_fw_version(uint8_t* fwversion) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;

  send_data[0] = CMD_LEN;
  send_data[1] = 0x00;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_GET_FW_VERSION;

  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(100);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + FWVER_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + FWVER_LEN + RECV_END_LEN);
    return 1;
  }
  dumpData("send_data", send_data, CMD_BASE_LEN);
  dumpData("recv_data", recv_data, RECV_LEN_LEN + FWVER_LEN + RECV_END_LEN);
  if (recv_data[RECV_LEN_LEN + 16] != 0x90 || recv_data[RECV_LEN_LEN + 16 + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 16 + RECV_END_LEN);
    return -1;
  }
  
  memcpy(fwversion, recv_data + RECV_LEN_LEN, FWVER_LEN);
  return 0;
}

/* get serial number
 * cmd: 0x80 0x81
 * Out: SN(24B) SW(2B) 
*/
int Securty_Element::cos_cmdhdlr_get_SN(uint8_t *SN) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  int err_ret=0;

  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_GET_SN;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + 32/*SN_LEN*/ + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 32/*SN_LEN*/ + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 32] != 0x90 || recv_data[RECV_LEN_LEN + 32 + 1] != 0x00) {
    //Serial.print("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 34 + RECV_END_LEN);
    return -1;
  }
  memcpy(SN, recv_data + RECV_LEN_LEN, 32/*SN_LEN*/);
  return 0;
}


/****** STKN Auth ******/

/* buddy initialization
 * cmd: 0x80 0xF0
 * Out: KMtrl(40B) SW(2B)
*/
int Securty_Element::cos_cmdhdlr_buddy_init(uint8_t* KMtrl)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_BUDDY_INIT;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + KMTRL_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + KMTRL_LEN + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 40] != 0x90 || recv_data[RECV_LEN_LEN + 40 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 40 + RECV_END_LEN);
    return -1;
  }
  memcpy(KMtrl, recv_data + 2, KMTRL_LEN);
  return 0;
}

/* get secure stub
 * cmd: 0x80 0xF1 0x00 [Part]
 * Out: EncSecStub(32B) SW(2B)
*/
int Securty_Element::cos_cmdhdlr_get_secstub(uint8_t Part, uint8_t* ESecstube)
{ 
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_GET_SECSTUB;
  send_data[4] = 0;   // P1
  send_data[5] = Part;  // P2
  
  if (send_data[5] > 32) {
    printf("Part number errror: part = %02X\r\n", send_data[5]);
    return 1;
  }
  dumpData("cos_cmdhdlr_get_secstub", send_data, 16);
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + ESecstube_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + ESecstube_LEN + RECV_END_LEN);
    return 1;
  }
  if(recv_data[2 + 32] != 0x90 || recv_data[2 + 32 + 1] != 0x00) {
    return 1;
  } else {
    memcpy(ESecstube, recv_data + 2, ESecstube_LEN);
    return 0;
  }
}

/* secure stub authentication
 * cmd: 0x80 0xF2 0x00 0x00 0x20
 * In : R1(16B) C2(16B)
 * Out: R2(16B) ESK(16B) SW(2B)
*/
int Securty_Element::cos_cmdhdlr_secstub_auth(uint8_t *response1, uint8_t *challenge2, uint8_t *response2, uint8_t *encSK)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN + 16 + 16; // total len = (6B) + response1(16B) + challenge2(16B)
  send_data[2] = CMD_CLA;
  send_data[3] = ES_SECSTUB_AUTH;
  send_data[4] = 3;                 // BDY KEY ID
  send_data[6] = 16 + 16;           // input data len = response1(16B) + challenge2(16B)
  
  memcpy(send_data + CMD_BASE_LEN, response1, 16);
  memcpy(send_data + CMD_BASE_LEN + 16, challenge2, 16);
  
  dumpData("cos_cmdhdlr_secstub_auth", send_data, 48);
  SPI_SND_FRAME(CMD_BASE_LEN + 16 + 16, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + 16 + 16 + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN + 16 + 16);
    dumpData("recv_data", recv_data, RECV_LEN_LEN  + 16 + 16 + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 32] != 0x90 || recv_data[RECV_LEN_LEN + 32 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 32 + RECV_END_LEN);
    return -1;
  }
  
  memcpy(response2, recv_data + RECV_LEN_LEN, 16);
  memcpy(encSK, recv_data + RECV_LEN_LEN + 16, 16);
  return 0;
}

/****** STKN Store ******/

/* write secure parameter (Non-wear-leveling) 
 * cmd: 0x80 0xF4 [BDY_SIDH] [SIDL] 0x20 0x00
 * In : EncParam(32B)
 * Out: SW(2B)
*/
int Securty_Element::cos_cmdhdlr_sec_param_write_nwl(uint8_t BDY_SIDH, uint8_t SIDL, uint8_t *writedata)
{
  uint16_t SID;
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  // check
  printf("BDY_SIDH = %02x, SIDL = %02x\r\n", BDY_SIDH, SIDL);
  SID = SIDL | (BDY_SIDH << 6);
  printf("SID = %04x = %d\r\n", SID, SID);
  
  send_data[0] = CMD_LEN + 32;  // + SID
  send_data[2] = CMD_CLA;
  send_data[3] = ES_SEC_PARAMTER_WRITE_NWL;
  send_data[4] = BDY_SIDH;          // p1
  send_data[5] = SIDL;              // p2
  send_data[6] = 32;            // SID
  memcpy(send_data + CMD_BASE_LEN, writedata, 32); // SID
  
  printf("send_data[4] = %02x, send_data[5] = %02x\r\n", send_data[4], send_data[5]);
  
  if (SID > 1024)
  {
    printf("Error at SID > 1024\r\n");
    return 1;
  }
  
  SPI_SND_FRAME(CMD_BASE_LEN + 32, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN + 32);
    dumpData("recv_data", recv_data, 4);
    return 1;
  }
  
  if (recv_data[2] != 0x90 || recv_data[3] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, 4);
    return -1;
  }
  
  return 0;
}

/* read secure parameter (Non-wear-leveling) 
 * cmd: 0x80 0xF5 [BDY_SIDH] [SIDL] 0x00 0x00
 * Out: EncParam(32B) SW(2B)
*/
int Securty_Element::cos_cmdhdlr_sec_param_read_nwl(uint8_t BDY_SIDH, uint8_t SIDL, uint8_t *readdata)
{
  uint16_t SID;
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  // check
  printf("BDY_SIDH = %02x, SIDL = %02x\r\n", BDY_SIDH, SIDL);
  SID = SIDL | (BDY_SIDH << 6);
  printf("SID = %04x = %d\r\n", SID, SID);
  
  send_data[0] = CMD_LEN; 
  send_data[2] = CMD_CLA;
  send_data[3] = ES_SEC_PARAMTER_READ_NWL;
  send_data[4] = BDY_SIDH;    // p1
  send_data[5] = SIDL;        // p2
  
  dumpData("send_data", send_data, CMD_BASE_LEN);
  
  if (SID > 1024)
  {
    printf("Error at SID > 1024\r\n");
    return 1;
  }
    
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + 32 + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 32 + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN + 32] != 0x90 || recv_data[RECV_LEN_LEN + 32 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 32 + RECV_END_LEN);
    return -1;
  }
  
  memcpy(readdata, recv_data + RECV_LEN_LEN, 32);
  return 0;
}

/* write secure parameter (wear-leveling) 
 * cmd: 0x80 0xF7 0x00 [ParamID] 0x20 0x00
 * In : EncParam(32B)
 * Out: SW(2B)
*/
int Securty_Element::cos_cmdhdlr_sec_param_write_wl(uint8_t ParamID, uint8_t *writedata)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  // check
  if (ParamID > 0x1F) 
  {
    printf("error at ParamID, ParamID = %02X\r\n", ParamID);
    return -1;
  }
  
  send_data[0] = CMD_LEN + PARAMETER_LEN; 
  send_data[2] = CMD_CLA;
  send_data[3] = ES_SEC_PARAMTER_WRITE_WL;
  send_data[5] = ParamID ;        // p2
  send_data[6] = PARAMETER_LEN;
  memcpy(send_data + CMD_BASE_LEN, writedata, 32);
  
  SPI_SND_FRAME(CMD_BASE_LEN + PARAMETER_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN + PARAMETER_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return -1;
  }
  return 0;
}

/* read secure parameter (wear-leveling) 
 * cmd: 0x80 0xF5 [BDY] [ParamID] 0x00 0x00
 * Out: EncParam(32B) SW(2B)
*/
int Securty_Element::cos_cmdhdlr_sec_param_read_wl(uint8_t BDY, uint8_t SID, uint8_t *readdata)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  // check
  if (SID > 0x1F) 
  {
    printf("error at ParamID, ParamID = %02X\r\n", SID);
    return -1;
  }
  
  send_data[0] = CMD_LEN; 
  send_data[2] = CMD_CLA;
  send_data[3] = ES_SEC_PARAMTER_READ_WL;
  send_data[4] = BDY;       // p1
  send_data[5] = SID;       // p2
      
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + PARAMETER_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + PARAMETER_LEN + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN + PARAMETER_LEN] != 0x90 || recv_data[RECV_LEN_LEN + PARAMETER_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + PARAMETER_LEN + RECV_END_LEN);
    return -1;
  }
  
  memcpy(readdata, recv_data + RECV_LEN_LEN, 32);
  return 0;
}


/****** Cipher Service ******/

/* get rand 
 * cmd: 0x80 0xC0
 * out: RAND(32B) SW(2B)
 */
int Securty_Element::cos_cmdhdlr_get_rand(uint8_t *rand) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_GET_RAND;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + RAND_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RAND_LEN + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 32 + 0 ] != 0x90 || recv_data[RECV_LEN_LEN + 32 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN +  + RECV_END_LEN);
    return -1;
  }
  memcpy(rand, recv_data + RECV_LEN_LEN, RAND_LEN);
  return 0;
}

/* set key 
 * cmd: 0x80 0xC1 0x02 [KID] 0x00 0x00
 * in : WKEY(16B)
 * out: SW(2B)
 */
int Securty_Element::cos_cmdhdlr_set_key(uint8_t keyID, uint8_t *key) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN + KEY_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_SET_KEY;
  send_data[4] = 2;         // P1:0(VDP), 2(ES)
  send_data[5] = keyID;
  
  memcpy(send_data + CMD_BASE_LEN, key, KEY_LEN);
  SPI_SND_FRAME(CMD_BASE_LEN + KEY_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN + KEY_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  }
  return 0;
}

/* reset key 
 * cmd: 0x80 0xC2 0x02 [KID]
 * out: SW(2B)
 */
int Securty_Element::cos_cmdhdlr_reset_key(uint8_t keyID) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_RESET_KEY;
  send_data[4] = 2;         // P1:0(VDP), 2(ES)
  send_data[5] = keyID;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  }
  return 0;
}

/* gen key 
 * cmd: 0x80 0xC3 0x02 [KID]
 * out: SW(2B)
 */
int Securty_Element::cos_cmdhdlr_gen_key(uint8_t keyID) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_GEN_KEY;
  send_data[4] = 2;         // P1:0(VDP), 2(ES)
  send_data[5] = keyID;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  }
  return 0;
}
 
/* get key 
 * cmd: 0x80 0xC4 0x02 [KID] 
 * out: WKEY(16B) SW(2B)
 */
int Securty_Element::cos_cmdhdlr_get_key(uint8_t keyID, uint8_t *key) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_GET_KEY;
  send_data[4] = 2;         // P1:0(VDP), 2(ES)
  send_data[5] = keyID;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + KEY_LEN + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + KEY_LEN + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + KEY_LEN] != 0x90 || recv_data[RECV_LEN_LEN + KEY_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + KEY_LEN + RECV_END_LEN);
    return -1;
  }
  
  memcpy(key, recv_data + RECV_LEN_LEN, KEY_LEN);
  return 0;
}
 
/* encrypt 
 * cmd: 0x80 0xC5 0x00 [KID] [inLenL] [inLenH]
 * in : InputData(~512B)
 * out: OutputData(~512B) SW(2B)
 */
int Securty_Element::cos_cmdhdlr_encrypt(uint8_t keyID, uint16_t dataLen, uint8_t *inputData, uint8_t *outputData) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN + dataLen;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_ENCRYPT;
  send_data[4] = 2;         // P1:0(VDP), 2(ES)
  send_data[5] = keyID;
  send_data[6] = dataLen & 0xFF;
  send_data[7] = (dataLen >> 8) & 0xFF;
  
  printf("[SPI]dataLen = %d, [inLenL] = %02X, [inLenH] = %02X\r\n", dataLen, send_data[6], send_data[7]);
  
  memcpy(send_data + CMD_BASE_LEN, inputData, dataLen);
  SPI_SND_FRAME(CMD_BASE_LEN + dataLen, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + dataLen + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + dataLen + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN + dataLen] != 0x90 || recv_data[RECV_LEN_LEN + dataLen + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + dataLen + RECV_END_LEN);
    return -1;
  }
  
  memcpy(outputData, recv_data + RECV_LEN_LEN, dataLen);
  return 0;
}
 
/* decrypt 
 * cmd: 0x80 0xC6 0x00 [KID] [inLenL] [inLenH]
 * in : InputData(~512B)
 * out: OutputData(~512B) SW(2B)
 */
int Securty_Element::cos_cmdhdlr_decrypt(uint8_t keyID, uint16_t dataLen, uint8_t *inputData, uint8_t *outputData) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN + dataLen;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_DECRYPT;
  send_data[4] = 2;         // P1:0(VDP), 2(ES)
  send_data[5] = keyID;
  send_data[6] = dataLen & 0xFF;
  send_data[7] = (dataLen >> 8) & 0xFF;
  
  printf("[SPI]dataLen = %d, [inLenL] = %02X, [inLenH] = %02X\r\n", dataLen, send_data[6], send_data[7]);
  
  memcpy(send_data + CMD_BASE_LEN, inputData, dataLen);
  SPI_SND_FRAME(CMD_BASE_LEN + dataLen, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + dataLen + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + dataLen + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN + dataLen] != 0x90 || recv_data[RECV_LEN_LEN + dataLen + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + dataLen + RECV_END_LEN);
    return -1;
  }
  
  memcpy(outputData, recv_data + RECV_LEN_LEN, dataLen);
  return 0;
}
 
/****** VDP Store ******/
/* getchallen 
 * cmd: 0x80 0xE0 0x00 0x00 0x00 0x00
 * out: challenge(16B) SW(2B)
 */
int Securty_Element::cos_cmdhdlr_vdp_getchallenge  (uint8_t *challenge)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_VDP_GETCHALLENG;
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + 16 + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 16 + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN + 16] != 0x90 || recv_data[RECV_LEN_LEN + 16 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 16 + RECV_END_LEN);
    return -1;
  } else {
    memcpy(challenge, recv_data + RECV_LEN_LEN, 32);
    return 0;
  }
}

/* auth 
 * cmd: 0x80 0xE1 0x00 0x00 0x20 0x00
 * in : response1(16B) challenge2(16B)
 * out: response2(16B) EncVDPSK(16B) SW(2B)
 */
int Securty_Element::cos_cmdhdlr_vdp_auth (uint8_t *response1, uint8_t *challenge2, uint8_t *response2, uint8_t *encVDPsk)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN + 16 + 16;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_VDP_AUTH;
  send_data[6] = 32;
  
  memcpy(send_data + CMD_BASE_LEN, response1, 16);
  memcpy(send_data + CMD_BASE_LEN + 16, challenge2, 16);
  
  SPI_SND_FRAME(CMD_BASE_LEN + 32, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + 32 + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 32 + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN + 32] != 0x90 || recv_data[RECV_LEN_LEN + 32 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 32 + RECV_END_LEN);
    return -1;
  } else {
    memcpy(response2, recv_data + RECV_LEN_LEN, 16);
    memcpy(encVDPsk,  recv_data + RECV_LEN_LEN + 16, 16);
    return 0;
  }
}

/* changekey 
 * cmd: 0x80 0xE2 0x00 0x00 0x10 0x00
 * in : EncVDPSK(16B)
 * out: SW(2B)
 */
int Securty_Element::cos_cmdhdlr_vdp_changekey (uint8_t *encVDPSK)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN + 16;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_VDP_CHANGEKEY;
  send_data[6] = 16;
  
  memcpy(send_data + CMD_BASE_LEN, encVDPSK, 16);
  
  SPI_SND_FRAME(CMD_BASE_LEN + 16, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(16, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  } else {
    return 0;
  }
}

/* writedata 
 * cmd: 0x80 0xE3 0x00 0x00 0x00 0x01
 * in : EncVDPData(256B)
 * out: SW(2B)
 */
int Securty_Element::cos_cmdhdlr_vdp_writedata (uint8_t *encVDPData)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);

  send_data[0] = CMD_LEN;
  send_data[1] = 0x01;      //256B
  send_data[2] = CMD_CLA;
  send_data[3] = ES_VDP_WRITEDATA;
  send_data[4] = 0x02;  // APDU P1
  send_data[5] = 0x01;  // APDU p2
  send_data[6] = 0x00;  // APDU LEN
  send_data[7] = 0x01;
  
  memcpy(send_data + CMD_BASE_LEN, encVDPData, 256);
  
  SPI_SND_FRAME(CMD_BASE_LEN + 256, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(16, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  } else {
    return 0;
  }
}

/* readdata 
 * cmd: 0x80 0xE4 0x00 0x00 0x00 0x00
 * out: EncVDPData(256B) SW(2B)
 */
int Securty_Element::cos_cmdhdlr_vdp_readdata (uint8_t *encVDPData)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);

  send_data[0] = CMD_LEN;
  send_data[1] = 0x00;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_VDP_READDATA;
  send_data[4] = 0x02;  // APDU P1
  send_data[5] = 0x01;  // APDU P2
  send_data[6] = 0x00;  // APDU LEN
  send_data[7] = 0x01;
    
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + 256 + RECV_END_LEN, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 256 + RECV_END_LEN);
    return 1;
  }
  
  if (recv_data[RECV_LEN_LEN + 256] != 0x90 || recv_data[RECV_LEN_LEN + 256 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 256 + RECV_END_LEN);
    return -1;
  } else {
    memcpy(encVDPData, recv_data + RECV_LEN_LEN, 256);
    return 0;
  }
}
 
/* reset 
 * cmd: 0x80 0xE5 0x00 0x00 0x00 0x00
 * out: SW(2B)
 */
 
int Securty_Element::cos_cmdhdlr_vdp_reset (uint8_t keyID)
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = ES_VDP_RESET; //ES_VDP_RESET!!!!!
  send_data[4] = 0;         // P1:0(VDP), 2(ES)
  send_data[5] = 0x00;      //keyID
  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN] != 0x90 || recv_data[RECV_LEN_LEN + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + RECV_END_LEN);
    return -1;
  }
  return 0;
}






int Securty_Element::cos_cmdhdlr_get_krp(uint8_t *krp)
{

    memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[1] = 0x00;
  send_data[2] = CMD_CLA;
  send_data[3] = 0xBE;

  
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(1000 * 120000);
  err_ret = SPI_RCV_FRAME(RECV_LEN_LEN + 96 + RECV_END_LEN, recv_data, 100);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, RECV_LEN_LEN + FWVER_LEN + RECV_END_LEN);
    return 1;
  }
  if (recv_data[RECV_LEN_LEN + 96] != 0x90 || recv_data[RECV_LEN_LEN + 96 + 1] != 0x00) {
    printf("Error at 90 00\r\n");
    dumpData("recv_data", recv_data, RECV_LEN_LEN + 16 + RECV_END_LEN);
    return -1;
  }
  
  memcpy(krp, recv_data + RECV_LEN_LEN, 96);
  return 0;

}

int Securty_Element::test_get_cos_data(uint8_t *cosdata) 
{
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  send_data[0] = CMD_LEN; 
  send_data[2] = CMD_CLA;
  send_data[3] = 0x87;
    
  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  err_ret = SPI_RCV_FRAME(2 + 512 + 2, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, 512 + 4);
    return 1;
  }
  memcpy(cosdata, recv_data + 2, 512);
  //dumpData("recv_data", recv_data, 512);
  
  return 0;
}

/* Cos Reset
 * cmd: 0x80 0xF9 
 */
int Securty_Element::cos_cmdhdlr_CosReset(void) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = COS_RESET;

  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  delayMicroseconds(frame_delays);
  /*
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 0;
  }
  */
  return 0;
}

/* Cos Preloader
 * cmd: 0x80 0x78 
 */
int Securty_Element::cos_cmdhdlr_BackLDR(void) {
  memset(send_data, 0, bufferLen);
  memset(recv_data, 0, bufferLen);
  
  send_data[0] = CMD_LEN;
  send_data[2] = CMD_CLA;
  send_data[3] = COS_BACK_LDR;

  SPI_SND_FRAME(CMD_BASE_LEN, send_data, frame_delays);
  //delayMicroseconds(frame_delays);
  /*
  err_ret = SPI_RCV_FRAME(SPI_FRAME_SIZE, recv_data, frame_delays);
  if(error_check(err_ret) != 0) 
  {
    dumpData("send_data", send_data, CMD_BASE_LEN);
    dumpData("recv_data", recv_data, SPI_FRAME_SIZE);
    return 0;
  }
  */
  return 0;
}


uint16_t apdu_lc(uint8_t *apdu)
{
  return apdu[4] + (apdu[5] << 8);
}

uint16_t apdu_le(uint8_t *apdu)
{
  uint16_t shift = (6 + apdu_lc(apdu));
  return (apdu[shift] + (apdu[shift+1] << 8));
}

int Securty_Element::Kv_Apdu_Transparent(uint8_t *data_in, uint16_t size_in, uint8_t *data_out, uint16_t size_out)
{

  uint16_t apdu_len = apdu_lc(data_in) + 6;
  uint16_t rsp_len = apdu_le(data_in);

  uint16_t delay = *((uint16_t *)(data_in + apdu_len + 2));
  uint16_t retry = *((uint16_t *)(data_in + apdu_len + 4));

  return apdu_send(data_in,apdu_len,data_out,rsp_len,delay,retry);
}

static int pow_int(int base, int power)
{
  int tmp = 1;
  int i = power;
  for (i = 0; i < power; ++i) {
    tmp *= base;
  }
  return tmp;
}



int Securty_Element::apdu_send(uint8_t *apdu, uint16_t apdu_len, uint8_t *apdursp, uint16_t lr, uint16_t delayms, uint16_t retry16b)
{

  #define BUFFER_SZ        1024

  uint8_t rspbuffer[BUFFER_SZ];
  const static uint32_t apdu_err90_sig = 0xee9090ee;

  int err_ret = 0;
  int frame_delays = 100;
  uint16_t snd_len, rcv_len;
  uint16_t retry = retry16b & 0x3FFF;    //the highest 2 bits is the scale of delay
  uint16_t delay_mlt = 1;   //total dealy nop : delay * mlt * 100
  uint8_t tx_buf[BUFFER_SZ];
  uint8_t rcv_buf[BUFFER_SZ];
  
  memset(tx_buf, 0, BUFFER_SZ);
  memset(rcv_buf, 0, BUFFER_SZ);
  // len 2bytes + cmd 6bytes + data nbytes
  snd_len = apdu_len + 2;
  //response len + resp data + sw len, SPI_RCV_FRAME bufferLength must larger then SPI_FRAME_SIZE
  rcv_len = RECV_LEN_LEN + lr + RECV_END_LEN;
  if (rcv_len < SPI_FRAME_SIZE)
    rcv_len = SPI_FRAME_SIZE;

  if (retry16b > 0x3FFF) {
    delay_mlt = pow_int(10, (retry16b >> 14) & 0x3);   //0x4XXX : 10, 0x8XXX : 100, 0xCxxx : 1000
    //Serial.println("delay_mlt %d\r\n");
  }

  memcpy(tx_buf, &apdu_len, 2);
  memcpy(tx_buf+2, apdu, apdu_len);
  //dumpData("ADPU tx buf", tx_buf, snd_len);
  while(retry-- > 0) {
    err_ret = SPI_SND_FRAME(snd_len, tx_buf, frame_delays);
    if (error_check(err_ret) != 0) {
      Serial.print("Error!");
      Serial.println(err_ret,HEX);
      if (retry > 0) {
        continue;
      } else {
        return 1;
      }
    }
    //Serial.print("m_sec_delay:");
    //Serial.println(delayms * delay_mlt);
    delay(delayms * delay_mlt);

    err_ret = SPI_RCV_FRAME(rcv_len, rcv_buf, frame_delays);    
    if (error_check(err_ret) != 0) {
      if (retry > 0) {
        continue;
      } 
      else {
        Serial.print("ADPU transfer error:");Serial.println(err_ret);
        if (rcv_buf[RECV_LEN_LEN + lr] != 0x90 || rcv_buf[RECV_LEN_LEN + lr + 1] != 0x00){
           return 1;
        }
      }
    }
    
    if (rcv_buf[RECV_LEN_LEN + lr] != 0x90 || rcv_buf[RECV_LEN_LEN + lr + 1] != 0x00) {
      Serial.println("Error at 90 00\r\n");
      dumpData("ADPU rx buf", rcv_buf, RECV_LEN_LEN + lr + RECV_END_LEN);
      if (rcv_buf[2] == 0x66 && rcv_buf[3] <= 0x57)
      {
          Serial.print("Error code:");Serial.print(rcv_buf[3],HEX);Serial.print("==>");Serial.print(error_names[rcv_buf[3]]);Serial.println("");
      }
      if (retry > 0) {
        continue;
      } else {
        //error sig. & status
        memcpy(apdursp, (uint8_t *)&apdu_err90_sig, 4);
        memcpy(apdursp + 4, rcv_buf, RECV_LEN_LEN + RECV_END_LEN);
        return -1;
      }
    }
  }
  //dumpData("ADPU rx buf", rcv_buf, RECV_LEN_LEN + lr + RECV_END_LEN);
  memcpy(apdursp, rcv_buf + RECV_LEN_LEN, lr);

  return 0;
}


void Securty_Element::dumpData(char* label, uint8_t *data, int len) {
  int i;
  Serial.print("=======================================================\r\n");
  Serial.print(label);
  for (i = 0; i < len; ++i) {
    if (i % 16 == 0) {
      Serial.print("\r\n\t");
    }
    if (data[i]<16)
    {
      Serial.print("0");
    }
    Serial.print(data[i],HEX);Serial.print(" ");
  }
  Serial.print("\r\n=======================================================\r\n");
  
}
