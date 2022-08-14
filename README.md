# 项目简介
- [B站视频演示](https://www.bilibili.com/video/BV1jB4y1h7Jz?share_source=copy_web&vd_source=a87486ca7ecd0a754606aaf5b7b2b5ff)
- 这是一个基于ESP8266制作的指纹锁，采用海凌科旗下的指纹模块FPM383C
- 另附STM32代码，使用Keil5编写，工程文件在 `Project` 目录下，名为 `工程模板.uvprojx`

# 界面配置
- 使用“点灯科技”APP新建一个WiFi设备，点击右上角“三个点”——“界面配置”，把目录里面的json数据复制粘贴到对话框里面，点击“更新配置”，然后退出软件，再打开就可以了。

# 注意事项
- 注意使用的是软串口，不是硬串口，不要看到单片机上有RX、TX就直接往上接。
- `**一定不能接错电源，V_TOUCH和VCC是接的3.3v！3.3v！3.3v！，TOUCHOUT是中断引脚，接在14口上，RX接在5口上，TX接在4口上，GND是接电源负极即GND。**`
- 程序我默认是上电不连接WiFi的，考虑到在电动车上是没有网络的情况下，一直循环去连接WiFI会耗时，这样指纹的识别速度就会变慢，所以我默认上电不开启WiFI，如果有需要注册、删除指纹等操作的时候再来连接WiFI使用手机操作。
- 开启WiFI有两种途径，一个是扫描错误的指纹大于等于5次就会自动开启WiFi连接，二是注册一个ID号为0的指纹，0号指纹可以用来开启WiFi。
- 如果想要上电就连接WiFi的话，只需要将第25行的变量`WiFi_Connected_State = 1`改为`0`即可。

# 使用方法
- 代码是使用VS Code里面的Platform IO插件写的 代码完全兼容Arduino IDE
- 如果使用Arduino IDE编译，可直接复制`src`目录下的`FPM383C.cpp`文件到Arduino IDE里面粘贴使用即可（注意添加相应的头文件，如`SoftwareSerial.h`、`Blinker.h`，也要删除`Arduino.h`）

# 其他文件
- PCB和3D模型图
- STM32的代码可用
- 上位机实测功能可用
- 参考协议手册也附上


