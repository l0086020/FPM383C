### 一、项目简介
- [BiliBili视频演示](https://www.bilibili.com/video/BV1jB4y1h7Jz?share_source=copy_web&vd_source=a87486ca7ecd0a754606aaf5b7b2b5ff)
- 基于ESP8266芯片、FPM383C指纹模块制作的简易电动车指纹锁
- 同时提供STM32、ESP32C3、Arduino程序源码
---

### 二、参考接线
|序号|FPM383C/D|STM32|ESP8266|ESP32C3|Arduino|
|:-:|:-:|:-:|:-:|:-:|:-:|
|1|V_Touch|+3.3V|+3.3V|+3.3V|+3.3V|
|2|Touch_OUT|PA0|GPIO14|GPIO2|D2|
|3|VCC|+3.3V|+3.3V|+3.3V|+3.3V|
|4|TXD|PA3|GPIO4|GPIO4|D4|
|5|RXD|PA2|GPIO5|GPIO5|D5|
|6|GND|GND|GND|GND|GND|
---

### 三、注意事项
- 若移植到如 `Arduino系列` 、 `STC系列` 等其他工作电压为 ` 5V ` 的单片机，务必转换串口电平，可参考[电平转换电路（未验证）](https://mp.weixin.qq.com/s/EIcMxW0amMTJZTIobH1mqA)，移植到以上平台时注意删除`Blinker`的所有相关函数，并且注意中断服务函数的要求
- 电源连接务必注意V_TOUCH和VCC的供电为 `3.3V`
- 默认初始化 `不连接WiFi` ，因考虑在电动车上没有网络，循环检测WiFI耗时会使指纹识别速度受到干扰，若需注册、删除指纹等操作时再连接WiFi使用手机操作
- 开启WiFI连接有两种方法：
  - 扫描错误的指纹≥5次；
  - 用ID为0的指纹来开启WiFi
- 若需初始化时连接WiFi，可将第25行的变量
  ```c
  WiFi_Connected_State = 1  改为  WiFi_Connected_State = 0
  ```
---

### 四、使用方法
- 程序使用Platform IO编写，代码完全兼容Arduino IDE
- 如果使用Arduino IDE开发环境，可复制 ` src ` 目录下的 ` FPM383C.cpp ` 文件里的代码到Arduino IDE里面粘贴使用（注意添加相应的头文件，如 ` SoftwareSerial.h ` 、 ` Blinker.h ` ）
---

### 五、App界面
- 使用 ` 点灯科技 ` APP新建一个WiFi设备，点击右上角 ` ... ` —— ` 界面配置 ` ，把目录里面的json数据复制粘贴到对话框里面，点击 ` 更新配置 ` 后退出软件，再次打开即可更新界面
---

### 六、其他文件
- 协议手册
- PC上位机（其中图像上传功能无法使用，其他功能正常）

---

### 七、其他说明
- 因代码后期改动可能与视频不符，请根据注释自行研究

---

### 八、帮助
- B站留言、私信
- QQ：321162955
- 微信：mr_zh-ang