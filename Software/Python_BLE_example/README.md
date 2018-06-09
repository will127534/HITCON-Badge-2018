## Python BLE interface 測試程式

總之這隻程式是為了測試BLE用的
使用方式如下:

首先把第8行的input_str替換成QR Code output的網址
接來就可以執行了
``` python
python3 ./tx.py TO_ADDRESS Amount GasValue GasLimit Noice Data
#TO_ADDRESS=>對方地址的HEX去掉0x
#Amount => 單位 eth
#GasValue => 單位gwei
#GasLimit => nothing special
#Noice =>交易數量
#Data => Raw Data Package
#如果要做token交易，請把TO-Address代入Token address，同時帶入ERC20的資料
#EX:sudo python3 ./tx.py 1e37af84b385e47b87f493ab187e6a764162a35a 0.1 50 50000 17 00
```

執行完之後會拿到簽章完畢的交易資料，如下圖
![](https://i.imgur.com/vMfoSq0.png)

