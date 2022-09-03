# 项目简介
- [BiliBili视频演示](https://www.bilibili.com/video/BV1jB4y1h7Jz?share_source=copy_web&vd_source=a87486ca7ecd0a754606aaf5b7b2b5ff)
- 基于ESP8266主控、FPM383C指纹模块制作的简易电动车指纹锁
- 另附STM32标准库代码，使用Keil5编写，工程文件在 ` Project ` 目录下，名为 ` 工程模板.uvprojx `；源代码在 `App` 目录下，名为 `FPM383C.c`

# 注意事项
- 若移植到如 `Arduino Uno` 、 `Arduino Nano` 、 `89C51` 、 `STC产品` 等其他工作电压为5V的单片机，务必转换串口电平，可参考[电平转换电路](https://mp.weixin.qq.com/s/EIcMxW0amMTJZTIobH1mqA)（未验证），移植到以上平台时注意删除`Blinker`的所有相关函数，并且注意中断服务函数的要求
- 需要注意使用的是Arduino提供的SoftwareSerial（软串口），在STM32代码中使用的是USART2
- 务必注意电源连接，**V_TOUCH和VCC的供电为 `3.3V`**，TOUCHOUT是触摸输出引脚，连接ESP8266 GPIO_Pin_14（EXTI），RX连接GPIO_Pin_5，TX连接GPIO_Pin_4，GND接地
- 默认初始化不连接WiFi，因考虑在电动车上没有网络，循环检测WiFI耗时会使指纹识别速度受到干扰，若需注册、删除指纹等操作时再连接WiFI使用手机操作
- 开启WiFI连接有两种方法：1、扫描错误的指纹≥5次；2、用ID为0的指纹来开启WiFi
- 若需初始化时连接WiFi，可将第25行的变量 ` WiFi_Connected_State = 1 ` 改为 ` WiFi_Connected = 0 `

# 使用方法
- 代码使用Visual Code - Platform IO插件编写，代码完全兼容Arduino平台
- 如果使用Arduino IDE开发环境，可复制 ` src ` 目录下的 ` FPM383C.cpp ` 文件里的代码到Arduino IDE里面粘贴使用（注意添加相应的头文件，如 ` SoftwareSerial.h ` 、 ` Blinker.h ` ，同时需删除 ` Arduino.h ` ）

# App界面
- 使用 ` 点灯科技 ` APP新建一个WiFi设备，点击右上角 ` 三个点 ` —— ` 界面配置 ` ，把目录里面的json数据复制粘贴到对话框里面，点击 ` 更新配置 ` 后退出软件，再次打开即可更新界面

# 其他文件
- PCB和3D模型图
- STM32的代码可用
- 上位机实测功能可用
- 参考协议手册也附上
