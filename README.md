# 项目简介
- [BiliBili视频演示](https://www.bilibili.com/video/BV1jB4y1h7Jz?share_source=copy_web&vd_source=a87486ca7ecd0a754606aaf5b7b2b5ff)
- 基于ESP8266主控、FPM383C指纹模块制作的简易电动车指纹锁
- STM32参考代码可通过串口调试进行指纹的注册、验证以及删除等操作

# 注意事项
- 若移植到如 `Arduino系列` 、 `STC系列` 等其他工作电压为 ` 5V ` 的单片机，务必转换串口电平，可参考[电平转换电路](https://mp.weixin.qq.com/s/EIcMxW0amMTJZTIobH1mqA)（未验证），移植到以上平台时注意删除`Blinker`的所有相关函数，并且注意中断服务函数的要求
- 需要注意使用的是Arduino提供的SoftwareSerial（软串口），在STM32代码中使用的是USART2
- 电源连接务必注意V_TOUCH和VCC的供电为 `3.3V`，TOUCHOUT是触摸输出引脚，连接ESP8266 GPIO_Pin_14（EXTI），RX连接GPIO_Pin_5，TX连接GPIO_Pin_4，GND接地
- 默认初始化不连接WiFi，因考虑在电动车上没有网络，循环检测WiFI耗时会使指纹识别速度受到干扰，若需注册、删除指纹等操作时再连接WiFi使用手机操作
- 开启WiFI连接有两种方法：1、扫描错误的指纹≥5次；2、用ID为0的指纹来开启WiFi
- 若需初始化时连接WiFi，可将第25行的变量 ` WiFi_Connected_State = 1 ` 改为 ` WiFi_Connected_State = 0 `

# 使用方法
- 代码使用Visual Code - Platform IO插件编写，代码完全兼容Arduino平台
- 如果使用Arduino IDE开发环境，可复制 ` src ` 目录下的 ` FPM383C.cpp ` 文件里的代码到Arduino IDE里面粘贴使用（注意添加相应的头文件，如 ` SoftwareSerial.h ` 、 ` Blinker.h ` ）

# App界面
- 使用 ` 点灯科技 ` APP新建一个WiFi设备，点击右上角 ` 三个点 ` —— ` 界面配置 ` ，把目录里面的json数据复制粘贴到对话框里面，点击 ` 更新配置 ` 后退出软件，再次打开即可更新界面

# 其他文件
- Altium Designer - PCB、Solidwork - 3D模型图
- 上位机实测功能可用（实测图像上传错误，其他功能正常）
- 参考协议手册

# 帮助
- B站留言、私信或QQ：321162955，看到会及时回复。