# SecureElement library

## Structure

wallet.cpp - Wallet application, like Creating new Account .. etc.  
Transaction.cpp - Code for transacting ETH, like RLP encoder and enc.  
SecurtyElement.cpp - For communicate with SE.   

## Demos

#### BackToInit
This demo is getting Secure Element back to Init state.
Note that BackToInit(); can only be exec in DISCONN mode.
So the Demo will move to DISCONN then exec BackToInit();

#### InitDevInfo
This demo is for setting up Secure Element from Init state
Again, InitDevInfo can only be exec in INIT mode. 
If the SE is not in INIT state, run BackToInit Demo to get SE in Init state.

#### HostRegistration
This demo is for Host Registration, it will save the credential to MT7697's Flash, and it will setup Pin code.
**Note that only the first reg host can login, so if you run this demo twice, it will clear the previous record and you cannnot login again**

#### HostLogin
This demo is for login SE.

#### InitWalletFromMnemonic
This demo is for initialize wallet using given Mnemonic, Mnemonic is written on the code.

#### CreateAccount
This demo is to creating an account and the first address.

#### QuaryAccountInfo
This demo is to quary the account info and its address.

#### Transaction
Demo for generatiing ETH raw transaction, note that the input is all HEX string, and it is **Necessary to append the HEX string length to even number**
If the Pin code is enabled, user will have to input the pin code.

## Misc

#### hardware_wallet.host_cred
Used as host cred, total length of 32bytes, in the example code and HitconBadge main program, MAC address in MT7697 is used as host cred.

#### GetModeState
pass a two bytes buffer, it will read the current state of SE,
the first byte is State, and the second one is status of last cmd.

State is defined as follow:
* 0x00 - INIT
* 0x01 - PERSO
* 0x02 - Normal
* 0x03 - AUTH
* 0x04 - LOCK
* 0x05 - ERROR
* 0x06 - NOHOST
* 0x07 - DISCONN

status of last cmd is 0x00 for success, status other than 0x00 is error code.
![](https://i.imgur.com/grEfyjS.png)
