/******************************************************************************************************************************
这是一个不完善的代码，只能实现注册指纹、验证指纹、清空指纹等几个基础功能，具体的功能完善需你们自己编写。
基于ESP8266的电动车指纹锁这个项目的目的是为了和大家一起学习ESP8266和FPM383C的串口驱动，本来只有一个ESP8266的代码的，后来受广大网友要求
我又添加了STM32标准库、HAL库、Arduino、ESP32等单片机的驱动程序，看懂代码的网友都已经发现，这几个单片机里的程序几乎都是一样的。我的目的是让
大家学习，不是让大家来白嫖的，别有事没事“UP能不能给我添加个这个功能 添加那个功能？”真有技术上的问题，欢迎加我QQ或私信，我都一一解答了。
*******************************************************************************************************************************/

#include "stdio.h"
#include "Arduino.h"
#include "SoftwareSerial.h" 

#define Bufferlength 20

/* 软串口，RX-GPIO4,TX-GPIO5 */
SoftwareSerial mySerial(4,5);

uint8_t FingerprintID,ScanState = 0;
uint8_t FPM383C_ReceiveBuffer[Bufferlength];

uint8_t FPM383C_SleepBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x33,0x00,0x37};
uint8_t FPM383C_EmptyBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};
uint8_t FPM383C_GetImageBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};
uint8_t FPM383C_CancelBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x30,0x00,0x34};
uint8_t FPM383C_GetChar1Buffer[13] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};
uint8_t FPM383C_BlueLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x03,0x01,0x01,0x00,0x00,0x49};
uint8_t FPM383C_RedLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x04,0x04,0x02,0x00,0x50};
uint8_t FPM383C_GreenLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x02,0x02,0x02,0x00,0x4C};
uint8_t FPM383C_SearchMBBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0xFF,0xFF,0x02,0x0C};
uint8_t FPM383C_AutoEnrollBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x31,'\0','\0',0x04,0x00,0x16,'\0','\0'}; //PageID: bit 10:11，SUM: bit 15:16
uint8_t FPM383C_DeleteBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,'\0','\0',0x00,0x01,'\0','\0'}; //PageID: bit 10:11，SUM: bit 14:15

void FPM383C_SendData(int len,uint8_t FPM383C_Databuffer[])
{
  mySerial.write(FPM383C_Databuffer,len);
  mySerial.flush();
}

uint8_t FPM383C_ReceiveData(uint16_t Timeout)
{
  uint8_t i = 0;
  while(mySerial.available() == 0 && (--Timeout))
  {
    delay(1);
  }
  while(mySerial.available())
  {
    delay(1);
    FPM383C_ReceiveBuffer[i++] = mySerial.read();
    if(i>=16) return 1;
  }
}

void FPM383C_Sleep(void)
{
  FPM383C_SendData(12,FPM383C_SleepBuffer);
}

void FPM383C_ControlLED(uint8_t FPM383C_ControlLEDBuffer[])
{
  FPM383C_SendData(16,FPM383C_ControlLEDBuffer);
}

uint8_t FPM383C_Cancel()
{
  FPM383C_SendData(12,FPM383C_CancelBuffer);
  FPM383C_ReceiveData(2000);
  return FPM383C_ReceiveBuffer[6] == 0x07 ? FPM383C_ReceiveBuffer[9] : 0xFF;
}

uint8_t FPM383C_GetImage()
{
  uint8_t ACK;
  FPM383C_SendData(12,FPM383C_GetImageBuffer);
  if(FPM383C_ReceiveData(2000))
  {
    if(FPM383C_ReceiveBuffer[6] == 0x07)
    {
      ACK = FPM383C_ReceiveBuffer[9];
      memset(FPM383C_ReceiveBuffer,0xFF,sizeof(FPM383C_ReceiveBuffer));
      return ACK;
    }
  }
  else
  {
    return 0xFF;
  }
}

uint8_t FPM383C_GetChar1(void)
{
  uint8_t ACK;
  FPM383C_SendData(13,FPM383C_GetChar1Buffer);
  if(FPM383C_ReceiveData(2000))
  {
    if(FPM383C_ReceiveBuffer[6] == 0x07)
    {
      ACK = FPM383C_ReceiveBuffer[9];
      memset(FPM383C_ReceiveBuffer,0xFF,sizeof(FPM383C_ReceiveBuffer));
      return ACK;
    }
  }
  else
  {
    return 0xFF;
  }
}

uint8_t FPM383C_SearchMB()
{
  FPM383C_SendData(17,FPM383C_SearchMBBuffer);
  FPM383C_ReceiveData(2000);
  return FPM383C_ReceiveBuffer[6] == 0x07 ? FPM383C_ReceiveBuffer[9] : 0xFF;
}

uint8_t FPM383C_Empty()
{
  FPM383C_SendData(12,FPM383C_EmptyBuffer);
  FPM383C_ReceiveData(2000);
  return FPM383C_ReceiveBuffer[6] == 0x07 ? FPM383C_ReceiveBuffer[9] : 0xFF;
}

uint8_t FPM383C_AutoEnroll(uint16_t PageID)
{
  FPM383C_AutoEnrollBuffer[10] = (PageID>>8);
  FPM383C_AutoEnrollBuffer[11] = (PageID);
  FPM383C_AutoEnrollBuffer[15] = (0x54+FPM383C_AutoEnrollBuffer[10]+FPM383C_AutoEnrollBuffer[11])>>8;
  FPM383C_AutoEnrollBuffer[16] = (0x54+FPM383C_AutoEnrollBuffer[10]+FPM383C_AutoEnrollBuffer[11]);
  FPM383C_SendData(17,FPM383C_AutoEnrollBuffer);
  FPM383C_ReceiveData(10000);
  return FPM383C_ReceiveBuffer[6] == 0x07 ? FPM383C_ReceiveBuffer[9] : 0xFF;
}

uint8_t FPM383C_Delete(uint16_t PageID)
{
  FPM383C_DeleteBuffer[10] = (PageID>>8);
  FPM383C_DeleteBuffer[11] = (PageID);
  FPM383C_DeleteBuffer[14] = (0x15+FPM383C_DeleteBuffer[10]+FPM383C_DeleteBuffer[11])>>8;
  FPM383C_DeleteBuffer[15] = (0x15+FPM383C_DeleteBuffer[10]+FPM383C_DeleteBuffer[11]);
  FPM383C_SendData(16,FPM383C_DeleteBuffer);
  FPM383C_ReceiveData(2000);
  return FPM383C_ReceiveBuffer[6] == 0x07 ? FPM383C_ReceiveBuffer[9] : 0xFF;
}

uint8_t FPM383C_Enroll(uint16_t PageID)
{
  if(FPM383C_AutoEnroll(PageID) == 0x00)
  {
    FPM383C_ControlLED(FPM383C_GreenLEDBuffer);
    return FPM383C_ReceiveBuffer[9];
  }
  FPM383C_ControlLED(FPM383C_RedLEDBuffer);
  return 0xFF;
}

uint8_t FPM383C_Identify()
{
  if(FPM383C_GetImage() == 0x00)
  {
    if(FPM383C_GetChar1() == 0x00)
    {
      if(FPM383C_SearchMB() == 0x00)
      {
        if(FPM383C_ReceiveBuffer[8] == 0x07 && FPM383C_ReceiveBuffer[9] == 0x00)
        {
          FPM383C_ControlLED(FPM383C_GreenLEDBuffer);
          return FPM383C_ReceiveBuffer[9];
        }
      }
    }
  }
  FPM383C_ControlLED(FPM383C_RedLEDBuffer);
  return 0xFF;
}

ICACHE_RAM_ATTR void InterruptFun()
{
  detachInterrupt(digitalPinToInterrupt(2));
  ScanState = 1;
}

void setup() 
{
  Serial.begin(115200);
  mySerial.begin(57600);

  delay(200);
  FPM383C_Sleep();
  delay(200);

  attachInterrupt(digitalPinToInterrupt(2),InterruptFun,RISING);
}

void loop() 
{
  if(ScanState)
  {
    FPM383C_ControlLED(FPM383C_BlueLEDBuffer);
    delay(10);
    
    FPM383C_Identify();   //验证指纹模式
    //FPM383C_Enroll(0);  //注册指纹模式

    while(digitalRead(2))
    {
      FPM383C_Sleep();
      delay(100);
    }
    ScanState = 0;
    attachInterrupt(digitalPinToInterrupt(2),InterruptFun,RISING);
  }
}
