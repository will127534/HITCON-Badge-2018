## Python BLE interface 測試程式

總之這隻程式是為了測試BLE用的
使用方式如下:

``` python
python3 ./tx.py TO_ADDRESS Amount GasValue GasLimit Noice Token_Toaddress(optional) Token_value(optional)
#TO_ADDRESS=>對方地址的HEX去掉0x
#Amount => 單位 eth
#GasValue => 單位gwei
#GasLimit => nothing special
#Noice =>交易數量
#
#如果要做token交易，請把TO-Address代入Token address
#另外注意的是Token method目前是寫Code裡面，所以需要跟換
#EX:python3 ./scan_ble.py 519A21E332329b86b1E19B25D977Dc87a14447CD 0.1 50 50000 17
#Token EX: sudo python3 ./tx.py 1e37af84b385e47b87f493ab187e6a764162a35a 0 50 100000 7 4bBb25f9220108C80B5F2902360E6987a85b10b7 1
```

執行完之後會拿到簽章完畢的交易資料，如下圖
![](https://i.imgur.com/YevB9ei.png)
