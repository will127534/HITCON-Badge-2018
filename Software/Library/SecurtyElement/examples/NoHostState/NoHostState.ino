#include <SPI.h>
#include <wallet.h>
#include <LWiFi.h>
#include "LFlash.h"

#define SE_SS 10

wallet hardware_wallet;
extern uint8_t vmk_new[VMK_LEN];
void setup(void) {

  Serial.begin(115200);



  hardware_wallet.init(SE_SS);
  init_se();

  delay(1000);
  waitforcmd();
  uint8_t ret = BackToNoHost();
//  uint8_t ret = BackToInitState();
  Serial.println(ret);
}


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
    ret = hardware_wallet.loader_cmdhdlr_KvVersion(buf);
    hardware_wallet.dumpData("Version",buf,16);
    if (ret == -1)
    {
        Serial.println("[init Secure Element] GetVersion Failed!");
    }
    else{
        Serial.println("[init Secure Element] GetVersion Success");
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
        }
        else{
            Serial.println("[init Secure Element] loader_cmdhdlr_CosJump Success");
        }
    }
    
    ret = hardware_wallet.GetModeState(buf);
    if (ret == -1)
    {
        Serial.println("[init Secure Element] GetModeState Failed!");
    }
    else{
        Serial.println("[init Secure Element] GetModeState Success");
    }
    Serial.print("[init Secure Element] Mode:");Serial.println(buf[0],HEX);
    Serial.print("[init Secure Element] State:");Serial.println(buf[1],HEX);

    if (buf[0] == 0x07) //DISCONN mode, login
    {
        Serial.println("[init Secure Element] DISCONN mode");
    }
    if (buf[0] == 0x06) //no host,init wallet
    {
        Serial.println("[init Secure Element] NOHOST mode");
    }
    if (buf[0] == 0x02) //Logged in already, do nothing
    {
        Serial.println("[init Secure Element] NORMAL mode");
    }
    if (buf[0] == 0x00) //INIT mode, init SE
    {
        Serial.println("[init Secure Element] INIT mode");
    }

    
}
void loop(void) {
  
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


int BackToNoHost()
{
  int ret = 0;
  uint8_t chlng[AUTHUTIL_CHLNG_LEN];
  ret = hardware_wallet.PIN_CHLNG(false,chlng);
  if (ret != 0)
    return ret;
  //if in login state, the 2nd parameter should not be NULL
  uint8_t pin[6] = {0x30,0x30,0x30,0x30,0x30,0x30};
  hardware_wallet.ModeAuth(pin, chlng);
  if (ret != 0)
    return ret;

  ret = hardware_wallet.BindBackToNoHost(chlng, NULL);
  if (ret != 0)
    return ret;
}

int BackToInitState(void)
{
  int ret = 0;
  uint8_t chlng[AUTHUTIL_CHLNG_LEN];
  uint8_t *curVMK;

  curVMK = vmk_new;

  ret = hardware_wallet.InitVMK_CHLNG(chlng);
  if (ret != 0)
    return ret;
  ret = hardware_wallet.InitBackInit(chlng, curVMK);
  if (ret != 0)
    return ret;

  return ret;
}