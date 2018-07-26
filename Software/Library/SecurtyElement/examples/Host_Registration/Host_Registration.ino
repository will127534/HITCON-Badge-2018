#include <SPI.h>
#include <wallet.h>
#include <LWiFi.h>
#include "LFlash.h"
#define SE_SS 10

wallet hardware_wallet;

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


  uint8_t ret = BindRegTest();
  Serial.println(ret);
}


void loop(void) {
  
}

int BindRegTest(void)
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
  hardware_wallet.dumpData("BindRegInit", rcv_buf, HANDLE_LEN + BIND_OTP_LEN);

  memcpy(brHandle, rcv_buf, HANDLE_LEN);
  memcpy(otp, rcv_buf + HANDLE_LEN, BIND_OTP_LEN);

  ret = hardware_wallet.BindRegChallenge(brHandle, chlng);
  if (ret != 0) {
    return ret;
  }
  hardware_wallet.dumpData("BindRegChallenge", chlng, BIND_CHLNG_LEN);

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