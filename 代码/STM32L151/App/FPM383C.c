/********************************************************************************************************************

v1.0因为使用的是While循环等待接收的数据，可能会因为数据接收出错或其他原因，导致程序卡死，或者将程序更改为while(--Timeout),用延时来等待接收数据。
v1.1使用IWDG独立看门狗2秒定时进行复位，这种方法可有效避免程序卡死。

中断引脚接在PC13上

在main函数里调用一下FPM383C_Init()就可以了。

********************************************************************************************************************/

#include "FPM383C.h"

//USART串口接收长度以及标志位
uint8_t USART2_STA  = 0;
__IO uint8_t USART3_STA	= 0;

//指纹ID和验证指纹的分数
uint16_t PageID,MatchScore;

//USART串口接收缓冲数组
uint8_t USART2_ReceiveBuffer[20];
uint8_t USART3_ReceiveBuffer[6];

//主循环状态标志位
uint8_t ScanStatus = 0;

//三种模式对应的命令
const char StringEnroll[7]  = "Enroll";
const char StringEmpty[7]   = "Empty";
const char StringDelete[7]  = "Delete";

//模块LED灯控制协议
uint8_t PS_BlueLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x03,0x01,0x01,0x00,0x00,0x49};
uint8_t PS_RedLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x04,0x04,0x02,0x00,0x50};
uint8_t PS_GreenLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x02,0x02,0x02,0x00,0x4C};

//模块睡眠协议
uint8_t PS_SleepBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x33,0x00,0x37};

//清空指纹协议
uint8_t PS_EmptyBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};

//取消命令协议
uint8_t PS_CancelBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x30,0x00,0x34};

//自动注册指纹协议
uint8_t PS_AutoEnrollBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x31,'\0','\0',0x04,0x00,0x16,'\0','\0'}; //PageID: bit 10:11，SUM: bit 15:16

//模块搜索用获取图像协议
uint8_t PS_GetImageBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};

//模块生成模板协议
uint8_t PS_GetChar1Buffer[13] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};
uint8_t PS_GetChar2Buffer[13] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};

//模块搜索指纹功能协议
uint8_t PS_SearchMBBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0xFF,0xFF,0x02,0x0C};

//删除指纹协议
uint8_t PS_DeleteBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,'\0','\0',0x00,0x01,'\0','\0'}; //PageID: bit 10:11，SUM: bit 14:15

/**
	* @brief	USART2串口发送函数
	* @param 	length: 发送数组长度
	* @param	FPM383C_Databuffer[]: 需要发送的功能协议数组，在上面已有定义
	* @return None
	*/
void FPM383C_SendData(int length,uint8_t FPM383C_Databuffer[])
{
	for(int i = 0;i<length;i++)
	{
		HAL_UART_Transmit(&huart2,(uint8_t *)&FPM383C_Databuffer[i],1,0xFFFF);
	}
	USART2_STA = 0;
}

/**
	* @brief	发送睡眠指令，只有发送睡眠指令后，TOUCHOUT才会变成低电平
	* @param 	None
	* @return None
	*/
void FPM383C_Sleep(void)
{
	FPM383C_SendData(12,PS_SleepBuffer);
}

/**
	* @brief	发送获取指纹图像指令
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_GetImage(uint32_t Timeout)
{
	uint8_t Temp;
	FPM383C_SendData(12,PS_GetImageBuffer);
	while(!USART2_STA && (--Timeout))
	{
		HAL_Delay(1);
	}
	Temp = (USART2_ReceiveBuffer[6] == 0x07 ? USART2_ReceiveBuffer[9] : 0xFF);
	memset(USART2_ReceiveBuffer,0xFF,sizeof(USART2_ReceiveBuffer));
	return Temp;
}


/**
	* @brief	发送生成模板指令
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_GenChar1(uint32_t Timeout)
{
	uint8_t Temp;
	FPM383C_SendData(13,PS_GetChar1Buffer);
	while(!USART2_STA && (--Timeout))
	{
		HAL_Delay(1);
	}
	Temp = (USART2_ReceiveBuffer[6] == 0x07 ? USART2_ReceiveBuffer[9] : 0xFF);
	memset(USART2_ReceiveBuffer,0xFF,sizeof(USART2_ReceiveBuffer));
	return Temp;
}


/**
	* @brief	发送搜索指纹指令
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_SearchMB(uint32_t Timeout)
{
	FPM383C_SendData(17,PS_SearchMBBuffer);
	while(!USART2_STA && (--Timeout))
	{
		HAL_Delay(1);
	}
	return (USART2_ReceiveBuffer[6] == 0x07 ? USART2_ReceiveBuffer[9] : 0xFF);
}


/**
	* @brief	删除指定指纹指令
  * @param  PageID ：需要删除的指纹ID号
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_Delete(uint16_t PageID,uint32_t Timeout)
{
	uint8_t Temp;
  PS_DeleteBuffer[10] = (PageID>>8);
  PS_DeleteBuffer[11] = (PageID);
  PS_DeleteBuffer[14] = (0x15+PS_DeleteBuffer[10]+PS_DeleteBuffer[11])>>8;
  PS_DeleteBuffer[15] = (0x15+PS_DeleteBuffer[10]+PS_DeleteBuffer[11]);
  FPM383C_SendData(16,PS_DeleteBuffer);
	while(!USART2_STA && (--Timeout))
	{
		HAL_Delay(1);
	}
	Temp = (USART2_ReceiveBuffer[6] == 0x07 ? USART2_ReceiveBuffer[9] : 0xFF);
	memset(USART2_ReceiveBuffer,0xFF,sizeof(USART2_ReceiveBuffer));
	return Temp;
}

/**
	* @brief	清空指纹库
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_Empty(uint32_t Timeout)
{
	uint8_t Temp;
	FPM383C_SendData(12,PS_EmptyBuffer);
	while(!USART2_STA && (--Timeout))
	{
		HAL_Delay(1);
	}
	Temp = (USART2_ReceiveBuffer[6] == 0x07 ? USART2_ReceiveBuffer[9] : 0xFF);
	memset(USART2_ReceiveBuffer,0xFF,sizeof(USART2_ReceiveBuffer));
	return Temp;
}


/**
	* @brief	发送控制灯光指令
	* @param 	PS_ControlLEDBuffer[]：不同颜色的协议数据
  * @param  Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_ControlLED(uint8_t PS_ControlLEDBuffer[],uint32_t Timeout)
{
	uint8_t Temp;
	FPM383C_SendData(16,PS_ControlLEDBuffer);
	while(!USART2_STA && (--Timeout))
	{
		HAL_Delay(1);
	}
	Temp = (USART2_ReceiveBuffer[6] == 0x07 ? USART2_ReceiveBuffer[9] : 0xFF);
	memset(USART2_ReceiveBuffer,0xFF,sizeof(USART2_ReceiveBuffer));
	return Temp;
}


/**
	* @brief	验证指纹流程
	* @param 	None
	* @return 确认码
	*/
void FPM383C_Identify(void)
{
	if(FPM383C_GetImage(2000) == 0x00)
	{
		if(FPM383C_GenChar1(2000) == 0x00)
		{
			if(FPM383C_SearchMB(2000) == 0x00)
			{
				HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_8);
				MatchScore = (int)((USART2_ReceiveBuffer[10] << 8) + USART2_ReceiveBuffer[11]);
				printf("\r\n扫描成功 指纹ID：%d\r\n",(int)MatchScore);
				FPM383C_ControlLED(PS_GreenLEDBuffer,1000);
				return;
			}
		}
	}
	printf("\r\n未搜索到指纹\r\n");
	FPM383C_ControlLED(PS_RedLEDBuffer,1000);
}

/**
	* @brief	注册指纹函数
	* @param 	PageID：输入需要注册的指纹ID号，取值范围0—59
	* @param	Timeout：设置注册指纹超时时间，因为需要按压四次手指，建议大于10000（即10s）
	* @return 确认码
	*/
void FPM383C_Enroll(uint16_t PageID,uint16_t Timeout)
{
	PS_AutoEnrollBuffer[10] = (PageID>>8);
  PS_AutoEnrollBuffer[11] = (PageID);
  PS_AutoEnrollBuffer[15] = (0x54+PS_AutoEnrollBuffer[10]+PS_AutoEnrollBuffer[11])>>8;
  PS_AutoEnrollBuffer[16] = (0x54+PS_AutoEnrollBuffer[10]+PS_AutoEnrollBuffer[11]);
  FPM383C_SendData(17,PS_AutoEnrollBuffer);
  while(!USART2_STA && (--Timeout))
	{
		HAL_Delay(1);
	}
	if(USART2_ReceiveBuffer[9] == 0x00)
	{
		printf("\r\n         >>>指纹注册成功<<<         \r\n");
		FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
		return;
	}
	else if(Timeout == 0)
	{
		FPM383C_SendData(12,PS_CancelBuffer);
		HAL_Delay(50);
	}
	FPM383C_ControlLED(PS_RedLEDBuffer,2000);
}
