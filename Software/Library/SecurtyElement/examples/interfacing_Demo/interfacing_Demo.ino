#include <SPI.h>
#include <SecurtyElement.h>


#define SE_SS 10

Securty_Element SE(SE_SS);

byte cmd1[] = {0x06, 0x00, 0x80, 0x11};
byte cmd2[] = {0x0A, 0x00, 0x80, 0xD0, 0x00 ,0x00,0x04,0x00,0x19,0x95,0x19,0x96};

void setup(void) {

  Serial.begin(115200);
  Serial.println("Hello!");

  SE.init();

  uint8_t recv_data[1024] = {0};
  SE.dumpData("send_cmd", cmd1, 4);
  SE.SpiSendPacket(4, cmd1, 100); 
  int recv_len;      
  uint8_t err_ret = SE.SpiRecvPacket(&recv_len, recv_data, 100);   
  SE.dumpData("recv_data", recv_data, recv_len);

  SE.dumpData("send_cmd", cmd2, 12);
  SE.SpiSendPacket(12, cmd2, 100); 
  err_ret = SE.SpiRecvPacket(&recv_len, recv_data, 100);   
  SE.dumpData("recv_data", recv_data, recv_len);
}


void loop(void) {
  
}
