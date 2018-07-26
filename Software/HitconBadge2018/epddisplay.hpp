#ifndef _epddisplay_hpp_
#define _epddisplay_hpp_
//e-Paper

#include <GxEPD.h>
#include <GxGDE0213B1/GxGDE0213B1.h>
//e-Paper
#include "wallet.hpp"

extern GxGDE0213B1 display;
#define total_page 4
#define MAINPAGE 1
#define SETTINGPAGE 2
#define TOKENPAGE 3
#define VCARDPAGE 4
#define ENT_BITS 128 //128-256 bits .multiple of 32 bits.
#define WORD_COUNT (ENT_BITS + (ENT_BITS/32))/11


void init_display();


//Battery_voltage ... etc
void display_static();
void main_menu();
void setting_menu();
void display_Vcard();
void BLE_pairing();
void display_paringQR();
void display_token_drawing();
bool Token_transaction_page(transaction_struct trx_test,String token_name);
bool transaction_page(transaction_struct *trx_test);
bool setpincode_page(uint8_t *pincode);
void pincode_error();
bool pincode_page(uint8_t *pincode,uint8_t retry);
bool confirm_pincode_page(uint8_t *pincode,uint8_t retry);
void pincode_success();
void success_animate();

void ReinitWallet();
String randomgeneratewallet();
String console_input_mnemonic();


void secureelement_error();
void display_initwalletFrommemonic();
void clear_ERC20();
#endif // _epddisplay_hpp_
