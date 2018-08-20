# HitconBadge Software

* **/Arduino_packages** - Folder for Arduino Package
* **/HitconBadge2018** - Main program
* **/Library** - Libraries used by Main Program
* **/Python_BLE_example** - Examples for BLE communication with Badge
* **/Utlity** - Some tools, bitmap to code and upload tool for Security Element and MT7697

## Arduino Package Usage:

Please go through the following step:
https://docs.labs.mediatek.com/resource/linkit7697-arduino/en/environment-setup/setup-arduino-ide

And replace board support package URL with : https://raw.githubusercontent.com/will127534/HITCON-Badge-2018/master/Software/Arduino_packages/package_hitcon_badge_index.json 

Libraries is included in the board support package, so you don't have to install the libraries manually.

## Libraries
* **Adafruit_GFX** - Graphics Library, Adafruit did a great gob on product and librarires and documents: https://learn.adafruit.com/adafruit-gfx-graphics-library/overview
* **Adafruit_PN532**	
* **CmdArduino** - Cmdline interface from https://github.com/fakufaku/CmdArduino, but I modified a bit to makes it compatiable with putty, also a larger buffer.  
* **GxEPD**	- From https://github.com/ZinggJM/GxEPD, but I also modified for Linkit7697 because some SPI interface issue. It is a bit large library designed for almost every intergrate controller E-ink display, the one that Hitconbadge used is GxGDE0213B1.
* **QRCode** - From https://github.com/ricmoo/QRCode, nothing is changed.
* **SecurtyElement** - From myself.

## Utlity

#### BMPtobinaryBitmap:
convert_Binary.py will convert every *.bmp files under the same folder to bitmap format in icon.c, so you can included in your program.

#### Upload tool:
This is use as a standalone upload tool for MT7697, which is the same as MT7697 Arduino Support Package. This tool is mainly for uploading bin files directly. There is both python and compiled exe for windows.
```
upload.exe -n da97.bin -c COM5 -f [bin file location]
```
#### SecureElement_Uploader:
This is the uploader for SLE97, MT7697 is used as a translator between python on PC and SLE97, so you will need to upload the translator bin **SLE97Loader.ino.bin** first, then do a power reset (un-plug and re-plug).
lastly, run the uploader as following:
```
python3 ./SLE97Loader.py. BINFILE COM_PORT
```
The bin file for SLE97 **SLE97_20180720.out** is included in the same folder.
