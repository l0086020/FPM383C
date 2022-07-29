# 项目简介
这是基于ESP8266制作的指纹锁，采用海凌科旗下的指纹模块FPM383C

# 注意事项
程序我默认是上电不连接WiFi的，考虑到在电动车上是没有网络的情况下，一直循环去连接WiFI会耗时，这样指纹的识别速度就会变慢，所以我默认上电不开启WiFI，如果有需要注册、删除指纹等操作的时候再来连接WiFI使用手机操作。开启WiFI有两种途径，一个是扫描错误的指纹大于等于5次就会自动开启WiFi连接，二是注册一个ID号为0的指纹，0号指纹可以用来开启WiFi。如果想要上电就连接WiFi的话，只需要将第25行的变量WiFi_Connected_State = 1改为0即可。

# 使用方法
代码是使用VS Code里面的Platform IO插件写的 代码完全兼容Arduino IDE，如果使用Arduino IDE编译，可直接复制src目录下的FPM383C.cpp文件到Arduino IDE里面粘贴使用即可（注意添加相应的头文件，如SoftwareSerial.h、Blinker.h，也要删除Arduino.h）

# 其他文件
PCB和3D模型图仅供参考
