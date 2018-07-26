#Hitcon Badge 2018 硬體架構
---
![](https://i.imgur.com/AyCMeHW.png)

幾個重要的Chip分別如下:
Main System on Chip: MT7697[官方網頁](https://labs.mediatek.com/zh-tw/chipset/MT7697)  
NFC Controller: PN532  
Display: Waveshare 2.13" [官方網頁](https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT)  
Secure Element: SLE97

MT7697的周邊是基於LinkIt 7697[(官方網頁)](https://docs.labs.mediatek.com/resource/mt7687-mt7697/en/get-started-linkit-7697-hdk) 開發版所做的二次開發，所以使用上如同LinkIt 7697

接下來分幾個子系統來說明  

### e-Paper:  

Waveshare 2.13" e-Paper 解析度為250x122的黑白雙色的電子紙且支援區域更新
和MT7697的接線圖如下表:

| E-Paper pin | MT7697 pin | Arduino |
| -------- | -------- | -------- |
| Busy     | GPIO34     | 4     |
| RST     | GPIO33     | 5     |
| D/C     | GPIO36     | 7     |
| /CS     | GPIO32     | 10     |
| SCK     | GPIO31     | 13     |
| MOSI     | GPIO29     | 11     |
  
電子紙的Graphics Library使用的是Adafruit GFX [(Link)](https://learn.adafruit.com/adafruit-gfx-graphics-library/overview)
Adafruit GFX提供了幾個畫圖的操作API，不過要注意的是只有黑白兩色  
而底層的電子紙控制則是用GxEPD Library[(Link)](https://github.com/ZinggJM/GxEPD)
另外就是由於電子紙的全域更新速度較慢，這個型號另外提供了一個區域更新的方法，也就是只更新部分區域的畫面  
但是要注意的是區域更新會有殘影留在螢幕上，所以幾次的區域更新之後需要一次全域更新去清除殘影。全域更新大約需要6s，而區域更新只需要<0.5s  

### NFC:
也是用Adafruit的Library [(Link)](https://github.com/adafruit/Adafruit-PN532)
和MT7697的接線圖如下表:

| PN532 | MT7697 pin | Arduino |
| -------- | -------- | -------- |
| I2C     | I2C     | I2C     |
| RST     | GPIO60     | 17     |
| INT     | GPIO0     | 2     |

  
### Secure Element:  
待Update
和MT7697的接線圖如下表:

| SLE97 | MT7697 pin | Arduino |
| -------- | -------- | -------- |
| SPI     | SPI     | SPI     |
| CS     | GPIO39     | 3     |

### Button Array:  
由於MT7697 IO不夠用了，所以矩陣鍵盤使用的是類比的方式，
藉由按鈕按下後所產生的不同組合的電阻產生的電壓去判定是哪一個按鈕被按下。
由於MT7697內建的ADC有4096階，使用上問題不大。
按鈕 A,B,C,0-9,DEL,ENTER都在Array裡面。  
| Button Array | MT7697 pin | Arduino |
| -------- | -------- | -------- |
| Analog Voltage     | GPIO58     | A1    |


### Special Buttons:  
由於MT7697設計上需要一個Bootstarp pin去切換Bootloader or User code，
所以GPIO37的Button在剛開機時作為強制切進Bootloader的方法，開機後就是一個普通的按鈕
另外一個按鈕則是RST按下之後MT7697會強制Reset。

| Function Name | MT7697 pin | Arduino |
| -------- | -------- | -------- |
| RST     | RST     | N/A     |
| Button D     | GPIO37     | 6     |

切進Bootloader的方式為:按下Reset和Button D，放開Reset之後放開Button D即可。
另外要注意的是，Button D按下之後 GPIO37是高電位，放開則低電位，所以邏輯上是按下為1，放開為0。

### LEDs:  

|  Name | Function |
| -------- | -------- |
| USR     | GPIO57,Arduino pin A0     |
| RX     | USB to UART RX訊號燈     |
| TX     | USB to UART TX訊號燈     |
| Power    | 電源指示燈     |

USR Led是一個簡單的LED接在MT7697上，IO拉高點亮，拉低則關閉。
另外Power LED旁邊有一個焊接的Jumper，可以把這個Jumper斷開以省電。

