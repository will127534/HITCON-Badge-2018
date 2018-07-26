#include "epddisplay.hpp"
#include "control.hpp"
#include "wallet.hpp"
#include <Cmd.h>
#include "cmdline_interface.hpp"
#include <ByteConverter.h>
#include "wallet.hpp"
#include "util.hpp"
#include <nvdm.h>
#include "LFlash.h"

char* HitconBanner[] = {" _______ __ __                         ______           __              ","|   |   |__|  |_.----.-----.-----.    |   __ \\.---.-.--|  |.-----.-----.","|       |  |   _|  __|  _  |     |    |   __ <|  _  |  _  ||  _  |  -__|","|___|___|__|____|____|_____|__|__|    |______/|___._|_____||___  |_____|","                                                           |_____|      ","                     ______ ______ ____   ______                        ","                    |__    |      |_   | |  __  |                       ","                    |    __|  --  |_|  |_|  __  |                       ","                    |______|______|______|______|                       ","                                                                        "};




void ReadVoltage_cmd(int arg_cnt, char **args){
	Serial.print("Voltage:");Serial.print(readBatteryVoltage());Serial.println("V");
}
void ReinitBLE_cmd(int arg_cnt, char **args){
	BLE_pairing();
}
void SE_state_cmd(int arg_cnt, char **args)
{
	uint8_t buf[2] ={0};
	int ret = hardware_wallet.GetModeState(buf);
    if (ret == -1)
    {
        Serial.println("GetModeState Failed!");
        return;
    }
    else{
        Serial.println("GetModeState Success");
    }
    Serial.print("Mode:");Serial.print(buf[0],HEX);Serial.print("==>");Serial.println(SE_Mode[buf[0]]);
    Serial.print("State:");Serial.println(buf[1],HEX);
}

void InitDevInfo_cmd(int arg_cnt, char **args)
{
	uint8_t ret = InitDevInfo();
	if (ret == 0)
	{
		Serial.println("Init Secure Element Success!!");
	}
	else{
		Serial.println("Init Secure Element Fail!!");
	}
}

void BindReg_cmd(int arg_cnt, char **args)
{
	uint8_t ret = BindReg();
	if (ret == 0)
	{
		Serial.println("BindReg Success!!");
	}
	else{
		Serial.println("BindReg Fail!!");
	}
}

void BindLogin_cmd(int arg_cnt, char **args)
{
	uint8_t ret = BindLogin(NULL);
	if (ret == 0)
	{
		Serial.println("BindLogin Success!!");
	}
	else{
		Serial.println("BindLogin Fail!!");
	}
}

void BindLogout_cmd(int arg_cnt, char **args)
{
	uint8_t ret = BindLogout();
	if (ret == 0)
	{
		Serial.println("BindLogout Success!!");
	}
	else{
		Serial.println("BindLogout Fail!!");
	}
}
void PIN_Auth_cmd(int arg_cnt, char **args){
	uint8_t pinCode[6];
    uint8_t pinCode_count = 0;
    Serial.print("[Transaction] Please enter Pincode:");
    while(1){
        char c = Serial.read();
        if (c == '\n' || c == '\r' && pinCode_count == 6)
        {
            Serial.println();
            break;
        }
        if (c <=0x39 && c >=0x30 && pinCode_count < 6) //numbers
        {
            Serial.write(c);
            pinCode[pinCode_count] = c;
            pinCode_count ++;
        }
        if (c == '\b' && pinCode_count>0) //backspace
        {
            pinCode_count --;
            Serial.write(c);
        }
    }
	int ret = PIN_AuthTest(pinCode);
	if (ret == 0)
	{
		Serial.println("PIN_AuthTest Success!!");
	}
	else{
		Serial.println("PIN_AuthTest Fail!!");
	}
}

void BackToNoHost_cmd(int arg_cnt, char **args)
{	
	Serial.print("This will clear host & wallet, are you sure to proceed(y/n)?");
	while(!Serial.available());
	char c = Serial.read();
	Serial.print(c);
	if (c != 'y')
	{
		return;
	}
	Serial.println();
	while(Serial.available()){
		char t = Serial.read(); //dump other data
	}
	uint8_t pinCode[6];
    uint8_t pinCode_count = 0;
    Serial.print("Please enter Pincode:");
    while(1){
        char c = Serial.read();
        if (c == '\n' || c == '\r' && pinCode_count == 6)
        {
            Serial.println();
            break;
        }
        if (c <=0x39 && c >=0x30 && pinCode_count < 6) //numbers
        {
            Serial.write(c);
            pinCode[pinCode_count] = c;
            pinCode_count ++;
        }
        if (c == '\b' && pinCode_count>0) //backspace
        {
            pinCode_count --;
            Serial.write(c);
        }
    }
	uint8_t ret = BackToNoHost(pinCode);
	if (ret == 0)
	{
		Serial.println("BackToNoHost Success!!");
	}
	else{
		Serial.println("BackToNoHost Fail!!");
	}
}
void BackToInit_cmd(int arg_cnt, char **args){
	Serial.print("This will CLEAR ALL DATA, are you sure to proceed(y/n)?");
	while(!Serial.available());
	char c = Serial.read();
	Serial.print(c);
	if (c != 'y')
	{
		return;
	}
	Serial.println();
	while(Serial.available()){
		char t = Serial.read(); //dump other data
	}

	uint8_t ret = BackToInit();
	if (ret == 0)
	{
		Serial.println("BackToInit Success!!");
	}
	else{
		Serial.println("BackToInit Fail!!");
	}
}

void hdw_query_all_account_info_cmd(int arg_cnt, char **args)
{
	uint8_t ret = hdw_query_all_account_info();
	if (ret == 1)
	{
		Serial.println("hdw_query_all_account_info Success!!");
	}
	else{
		Serial.println("hdw_query_all_account_info Fail!!");
	}
}

void hdw_query_wallet_info_cmd(int arg_cnt, char **args)
{
	uint8_t ret = hdw_query_wallet_info();
	if (ret == 0)
	{
		Serial.println("query_wallet Success!!");
	}
	else{
		Serial.println("query_wallet Fail!!");
	}
}

void hdw_query_account_key_info_cmd(int arg_cnt, char **args)
{
	uint32_t account,key;
	int i = 0;
	i = sscanf(args[1], "%d", &account);

	i += sscanf(args[2], "%d", &key);
	if (i != 2)
	{
		Serial.println("Format error! QueryAccountKey [account] [key_id]");
		return;
	}
	uint8_t ret = hdw_query_account_key_info(account,key);
	if (ret == 0)
	{
		Serial.println("QueryAccountKey Success!!");
	}
	else{
		Serial.println("QueryAccountKey Fail!!");
	}
}

void hdw_create_account_cmd(int arg_cnt, char **args){
	//int hdw_create_account(uint32_t cointype,uint32_t account,char* account_name)
	uint32_t cointype,account;
	if (arg_cnt != 4)
	{
		Serial.println("Format error! CreateAccount [cointype] [account_id] [Name]");
		return;
	}
	sscanf(args[1], "%d", &cointype);
	sscanf(args[2], "%d", &account);
	uint8_t ret = hdw_create_account(cointype,account,args[3]);
	if (ret == 0)
	{
		Serial.println("CreateAccount Success!!");
	}
	else{
		Serial.println("CreateAccount Fail!!");
	}
}

void hdw_query_account_info_cmd(int arg_cnt, char **args){
	uint32_t account;
	
	if (arg_cnt != 2)
	{
		Serial.println("Format error! QueryAccountInfo [account_id]");
		return;
	}
	sscanf(args[1], "%d", &account);

	uint8_t ret = hdw_query_account_info_cmdline(account);
	if (ret == 0)
	{
		Serial.println("QueryAccountInfo Success!!");
	}
	else{
		Serial.println("QueryAccountInfo Fail!!");
	}

}
void hdw_generate_next_trx_addr_cmd(int arg_cnt, char **args){
	uint32_t account;
	int i = 0;
	if (arg_cnt != 2)
	{
		Serial.println("Format error! CreateNextAddr [account_id]");
		return;
	}
	sscanf(args[1], "%d", &account);
	uint8_t ret = hdw_generate_next_trx_addr_account(account);
	if (ret == 0)
	{
		Serial.println("CreateNextAddr Success!!");
	}
	else{
		Serial.println("CreateNextAddr Fail!!");
	}
}

void InitWallet_cmd(int arg_cnt, char **args){
	if (arg_cnt == 12+1 || arg_cnt == 15+1 || arg_cnt == 18+1 || arg_cnt == 21+1 || arg_cnt == 24+1){
		String mnemonic;
		for (int i = 1; i < arg_cnt; ++i)
		{
			mnemonic += String(args[i]);
			if (i <= arg_cnt-1)
			{
				mnemonic += " ";
			}
		}
		Serial.print("Mnemonic words:");Serial.println(mnemonic);
		InitWalletFromMnemonic(mnemonic);
	}
	else if (arg_cnt == 12+2 || arg_cnt == 15+2 || arg_cnt == 18+2 || arg_cnt == 21+2 || arg_cnt == 24+2){
		String mnemonic;
		for (int i = 1; i < arg_cnt; ++i)
		{
			mnemonic += String(args[i]);
			if (i <= arg_cnt-1)
			{
				mnemonic += " ";
			}
		}
		Serial.print("Mnemonic words:");Serial.println(mnemonic);
		String passphrase = String(args[arg_cnt-1]);
		Serial.print("Passphrase:");Serial.println(passphrase);
		InitWalletFromMnemonic_withpassphrase(mnemonic,passphrase);
	}
	else{
		Serial.print("Mnemonic Word length error! length:");Serial.println(arg_cnt);
		return;
	}

}

void Transaction_cmd(int arg_cnt, char **args){
	if (arg_cnt != 9)
	{
		Serial.println("Format error! (Append hex value to even number) Transaction [account_id]/[key_id] [To(hex)] [Value(hex)] [Gas Price(hex)] [Gas Limit(hex)] [Nonce(hex)] [Data(hex)]");
		return;
	}
	/*
	struct transaction_struct
		  std::vector<uint8_t> to;
		  std::vector<uint8_t> value;
		  std::vector<uint8_t> gasPrice;
		  std::vector<uint8_t> gasLimit;
		  std::vector<uint8_t> nonce;
		  std::vector<uint8_t> data;
	*/
	transaction_struct trx;
	trx.to = ByteConverter::charStrToBytes((const uint8_t *)args[3]);
	trx.value = ByteConverter::charStrToBytes((const uint8_t *)args[4]);
	trx.gasPrice = ByteConverter::charStrToBytes((const uint8_t *)args[5]);
	trx.gasLimit = ByteConverter::charStrToBytes((const uint8_t *)args[6]);
	trx.nonce = ByteConverter::charStrToBytes((const uint8_t *)args[7]);
	trx.data = ByteConverter::charStrToBytes((const uint8_t *)args[8]);

	uint32_t account;
	sscanf(args[1], "%d", &account);
	uint32_t key;
	sscanf(args[2], "%d", &key);
	Serial.print("Account:");Serial.print(account);Serial.print("/Key_ID:");Serial.println(key);
	Serial.print("To: 0x");Serial.println(vector2string(trx.to));
	Serial.print("Value: ");Serial.print(vector2string_DEC(trx.value,2));Serial.println("ETH");
	Serial.print("gasPrice: ");Serial.print(vector2string_DEC(trx.gasPrice,1));Serial.println("Gwei");
	Serial.print("gasLimit: ");Serial.println(vector2string_DEC(trx.gasLimit,0));
	Serial.print("Nonce: ");Serial.println(vector2string_DEC(trx.nonce,0));
	Serial.print("Data: ");Serial.println(vector2string(trx.data));

	uint8_t output_raw[256] = {0};
	//uint8_t start_transaction_cmdline(uint8_t* outputdata,uint32_t account,uint32_t key,transaction_struct trx_data);
	uint32_t length = start_transaction_cmdline(output_raw,account,key,trx);
	Serial.print("Raw Transaction:");Serial.println(ArraytoString(output_raw,length));

}

String read_string(){
	String data;
	uint32_t length_limit =  100;
	uint32_t counter = 0;
	while(1){
		while(!Serial.available()){
			delay(10);
		}
	    char c = Serial.read();
	    if (c < 0x20 && c != '\r' && c != '\b')
	    {
	        continue;
	    }
	    switch (c)
	    {
		    case '\r':
		    	Serial.println();
		        return data;
		        break;
		    case '\b':
		        // backspace 

		        if (counter > 0)
		        {
		        	Serial.write(c);
		            data.remove(data.length()-1);
		            counter--;
		        }
		        break;
		    case '\0':
		        // do noting 
		        break;
		    default:
		        // normal character entered. add it to the buffer
			    if (counter < length_limit)
			    {
			    	Serial.write(c);
			        data += c;
			        counter++;
			    }
		        break;
	    }
	}
}

void AddVcard_cmd(int arg_cnt, char **args){
	Serial.println("Adding VCARD, please input some persional info, enter to skip or enter data");
	Serial.print("Please input your name(English only..):");
	String name = read_string();
	Serial.print("Please input your phone number:");
	String phone = read_string();
	Serial.print("Please input your EMAIL:");
	String email = read_string();
	Serial.print("Company Name?:");
	String company = read_string();
	Serial.print("Website?:");
	String website = read_string();
	
	Serial.print("linkedin url?:");
	String linkedin = read_string();
	Serial.print("facebook url?:");
	String facebook = read_string();
	Serial.print("twitter url?:");
	String twitter = read_string();
	

	String Vcard;
	Vcard += "BEGIN:VCARD\r\n";
	Vcard += "N:";Vcard += name;Vcard += "\r\n";
	if (company.length() != 0)
	{
		Vcard += "ORG:";Vcard += company;Vcard += "\r\n";
	}
	if (email.length() != 0)
	{
		Vcard += "EMAIL;type=INTERNET;type=WORK;type=pref:";Vcard += email;Vcard += "\r\n";
	}
	if (phone.length() != 0)
	{
		Vcard += "TEL;type=CELL:";Vcard += phone;Vcard += "\r\n";
	}
	if (website.length() != 0)
	{
		Vcard += "item1.URL:";Vcard += website;Vcard += "\r\n";
	}
	
	if (linkedin.length() != 0)
	{
		Vcard += "socialProfile;type=linkedin:";Vcard += linkedin;Vcard += "\r\n";
	}	
	if (facebook.length() != 0)
	{
		Vcard += "socialProfile;type=facebook:";Vcard += facebook;Vcard += "\r\n";
	}		
	if (twitter.length() != 0)
	{
		Vcard += "socialProfile;type=twitter:";Vcard += twitter;Vcard += "\r\n";
	}
	
	Vcard += "END:VCARD";


	Serial.println(Vcard);
	Serial.print("Total Length:");Serial.print(Vcard.length());Serial.println("Bytes");
	if (Vcard.length()>230)
	{
		Serial.println("Length too large, limit is 230 byte");
		return;
	}
	nvdm_status_t status;
    status = nvdm_write_data_item("VCARD", "VCARD",  NVDM_DATA_ITEM_TYPE_STRING , (uint8_t *)Vcard.c_str(),Vcard.length()+1);
    if (status != NVDM_STATUS_OK) {
        Serial.println("Write Error!");
    }
    status = nvdm_write_data_item("VCARD", "Name",  NVDM_DATA_ITEM_TYPE_STRING , (uint8_t *)name.c_str(),name.length()+1);
    if (status != NVDM_STATUS_OK) {
        Serial.println("Write Error!");
    }


}

void cmdline_interface_process(){
	cmdPoll();
}
void hello(int arg_cnt, char **args)
{
  Serial.println("Hello world.");
}
void ResetHitconTokenDisplay(int arg_cnt, char **args)
{
	double HITCON_Display_Balance;
  	uint32_t size = 8;
	HITCON_Display_Balance = 0;
	LFlash.write("Wallet","HitconDisplayBalance",LFLASH_RAW_DATA,(const uint8_t *)&HITCON_Display_Balance,sizeof(HITCON_Display_Balance));
  	Serial.println("Deleted");
}



void help(int arg_cnt, char **args){
	Serial.println("************************");
	Serial.println("*       CMD List       *");
	Serial.println("************************");
		Serial.println("?/h/help: Show CMD List");
		Serial.println("SEState: Query Secure Element State");
		Serial.println("BindReg: (This will clean the previous binding data!!)Bind Register");
		Serial.println("BindLogin: Bind Login");
		Serial.println("BindLogout: Bind Logout");
		Serial.println("BackToNoHost: (Clear host & wallet!!!)Back to NoHost state");
		Serial.println("BackToInit: (Clear all!!!)Back to Init State");
		Serial.println("InitDevInfo: InitDeviceInfo and confirm");
		Serial.println("PINAuth: Auth Pin Code");
		Serial.println("");
		Serial.println("InitWallet: Initialize Wallet from Mnemonic");
		Serial.println("Usage: InitWallet [Mnemonic word list]");
		Serial.println("");
		Serial.println("QueryWalletInfo: Query Wallet Info");
		Serial.println("QueryAllAccount: Query All Account Info");
		Serial.println("");
		Serial.println("QueryAccountKey: Query Public Key for a specific Account/Key_ID"); 
		Serial.println("Usage: QueryAccountKey [account] [key_id]");
		Serial.println("");
		Serial.println("CreateAccount: Create a New Account for a specific cointype/account_id");
		Serial.println("Usage: CreateAccount [cointype] [account_id] [Name]");
		Serial.println("");
		Serial.println("QueryAccountInfo: Query a specific Account Info");
		Serial.println("Usage: QueryAccountInfo [account_id]");
		Serial.println("");
		Serial.println("CreateNextAddr: Generate a new key for a Specific Account");
		Serial.println("Usage: CreateNextAddr [account_id]");
		Serial.println("");
		Serial.println("Transaction: Generate a new Raw Transaction");
		Serial.println("Usage: (Append hex value to even number) Transaction [account_id]/[key_id] [To(hex)] [Value(hex)] [Gas Price(hex)] [Gas Limit(hex)] [Nonce(hex)] [Data(hex)]");
		Serial.println("");

		Serial.println("Voltage: Read Battery Voltage");
		Serial.println("ReinitBLE: Re-init BLE");
		Serial.println("AddVcard: Adding a Vcard");
		Serial.println("ResetHitconTokenDisplay: Reset Hitcon Token counter");
}

void Nop(int arg_cnt, char **args){

}
void init_cmdline_interface(){
	cmdInit(115200);
	cmdAdd("help", help);
	cmdAdd("h", help);
	cmdAdd("?", help);
	cmdAdd("", Nop);
	cmdAdd("hello", hello);

	cmdAdd("BackToNoHost", BackToNoHost_cmd);
	cmdAdd("InitDevInfo", InitDevInfo_cmd);
	cmdAdd("SEState", SE_state_cmd);
	cmdAdd("BindReg", BindReg_cmd);
	cmdAdd("BindLogin", BindLogin_cmd);
	cmdAdd("BindLogout", BindLogout_cmd);
	cmdAdd("BackToNoHost", BackToNoHost_cmd);
	cmdAdd("BackToInit",BackToInit_cmd);
	cmdAdd("InitWallet", InitWallet_cmd);
	cmdAdd("PINAuth", PIN_Auth_cmd);
	cmdAdd("QueryWalletInfo", hdw_query_wallet_info_cmd);
	cmdAdd("QueryAllAccount", hdw_query_all_account_info_cmd);
	cmdAdd("QueryAccountKey", hdw_query_account_key_info_cmd);
	cmdAdd("CreateAccount", hdw_create_account_cmd);
	cmdAdd("QueryAccountInfo", hdw_query_account_info_cmd);
	cmdAdd("CreateNextAddr", hdw_generate_next_trx_addr_cmd);
	
	//cmdAdd("SetSecurityPolicy", SetSecurityPolicy_cmd);
	cmdAdd("Transaction", Transaction_cmd);

	cmdAdd("Voltage", ReadVoltage_cmd);
	cmdAdd("ReinitBLE", ReinitBLE_cmd);
	cmdAdd("AddVcard", AddVcard_cmd);
	cmdAdd("ResetHitconTokenDisplay", ResetHitconTokenDisplay);

	Serial.println("**************************************************************************");
	for (int i = 0; i < 10; ++i)
	{
		Serial.print("*");Serial.print(HitconBanner[i]);Serial.println("*");
	}
	Serial.println("**************************************************************************");
	Serial.println("Hitcon Badge 2018 Cmdline interface\r\nVersion:1.0.12\r\nenter help to see the cmd list");
	cmd_display();
}

