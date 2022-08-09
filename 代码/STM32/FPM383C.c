/********************************************************************************************************************
v1.0因为使用的是While循环等待接收的数据，可能会因为数据接收出错或其他原因，导致程序卡死，或者将程序更改为while(--Timeout),用延时来等待接收数据。
v1.1使用IWDG独立看门狗2秒定时进行复位，这种方法可有效避免程序卡死。

此示例只进行了简单的验证指纹，注册函数都没有写，可以借鉴我的ESP8266写的代码，GitHub：https://github.com/l0086020/FPM383C.git

中断引脚接在PC13上
.h文件里没有包含其他东西，就是函数原型，所以就不上传了。

在main函数里调用一下FPM383C_Init()就可以了。

可能要加上USART1的初始化，因为有调试信息输出，也可以把void ACK(uint8_t ack);里面的所有的Printf全删了都可以。

需要包含delay文件、stm32芯片头文件、NVIC中断向量等等，就是缺啥自己加啥，指纹模块的功能都是可用的。
********************************************************************************************************************/

#include "main.h"

uint8_t Receive_STA = 0;
uint16_t PageID,MatchScore;
uint8_t FPM383C_ReceiveBuffer[20];

//模块LED灯控制协议
uint8_t PS_BlueLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x03,0x01,0x01,0x00,0x00,0x49};
uint8_t PS_RedLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x04,0x04,0x02,0x00,0x50};
uint8_t PS_GreenLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x02,0x02,0x02,0x00,0x4C};

//模块睡眠协议
uint8_t PS_SleepBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x33,0x00,0x37};

//模块搜索用获取图像协议
uint8_t PS_GetImageBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};

//模块生成模板协议
uint8_t PS_GetChar1Buffer[13] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};
uint8_t PS_GetChar2Buffer[13] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};

//模块搜索指纹功能协议
uint8_t PS_SearchMBBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0xFF,0xFF,0x02,0x0C};

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
		USART_SendData(USART2,FPM383C_Databuffer[i]);
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
	}
	Receive_STA = 0;
}


/**
	* @brief	发送睡眠指令，只有发送睡眠指令后，TOUCHOUT才会变成低电平
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_Sleep(uint32_t Timeout)
{
	FPM383C_SendData(12,PS_SleepBuffer);
	while(Receive_STA<12 && (--Timeout))
	{
		delay_ms(1);
	}
	return FPM383C_ReceiveBuffer[9];
}


/**
	* @brief	发送获取指纹图像指令
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_GetImage(uint32_t Timeout)
{
	FPM383C_SendData(12,PS_GetImageBuffer);
	while(Receive_STA<12 && (--Timeout))
	{
		delay_ms(1);
	}
	return FPM383C_ReceiveBuffer[9];
}


/**
	* @brief	发送生成模板指令
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_GenChar1(uint32_t Timeout)
{
	FPM383C_SendData(13,PS_GetChar1Buffer);
	while(Receive_STA<12 && (--Timeout))
	{
		delay_ms(1);
	}
	return FPM383C_ReceiveBuffer[9];
}


/**
	* @brief	发送生成模板指令，暂时未用到
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_GenChar2(uint32_t Timeout)
{
	FPM383C_SendData(13,PS_GetChar2Buffer);
	while(Receive_STA<12 && (--Timeout))
	{
		delay_ms(1);
	}
	return FPM383C_ReceiveBuffer[9];
}


/**
	* @brief	发送搜索指纹指令
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_SearchMB(uint32_t Timeout)
{
	FPM383C_SendData(17,PS_SearchMBBuffer);
	while(Receive_STA<16 && (--Timeout))
	{
		delay_ms(1);
	}
	return FPM383C_ReceiveBuffer[9];
}


/**
	* @brief	发送控制灯光指令
	* @param 	PS_ControlLEDBuffer[]：不同颜色的协议数据
	* @return None
	*/
void FPM383C_ControlLED(uint8_t PS_ControlLEDBuffer[])
{
	FPM383C_SendData(16,PS_ControlLEDBuffer);
}


/**
	* @brief	验证指纹流程
	* @param 	None
	* @return 确认码or无效码
	*/
uint8_t FPM383C_Identify()
{
	if(FPM383C_GetImage(2000) == 0x00)
	{
		if(FPM383C_GenChar1(2000) == 0x00)
		{
			if(FPM383C_SearchMB(2000) == 0x00)
			{
				if(FPM383C_ReceiveBuffer[8] == 0x07 && FPM383C_ReceiveBuffer[9] == 0x00)
				{
					FPM383C_ControlLED(PS_GreenLEDBuffer);
					return FPM383C_ReceiveBuffer[9];
				}
			}
		}
	}
	FPM383C_ControlLED(PS_RedLEDBuffer);
	return 0xFF;
}

/**
	* @brief	清空指纹库，还没有写，可以参考一下我的ESP8266的代码，GitHub链接：https://github.com/l0086020/FPM383C.git
	* @param 	None
	* @return 确认码
	*/
uint8_t FPM383C_Empty()
{
	//FPM383C_SendData(12,FPM383C_EmptyBuffer);
	while(Receive_STA<12);
	return FPM383C_ReceiveBuffer[9];
}


/**
	* @brief	判断接收的确认码并执行相应的功能操作
	* @param 	ack：各个功能函数返回的确认码
	* @return None
	*/
void ACK(uint8_t ack)
{
	if(FPM383C_ReceiveBuffer[6] == 0x07)
	{
		switch (ack)
		{
			case 0x00:
				printf("指令正确，回传数据 : 0x0%x\r\n",ack);				//指令正确，下面是实际需要操作的东西，比如控制继电器，打开关闭灯等等。
				GPIOB->ODR ^= 1<<0;
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(50);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				delay_ms(20);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(50);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
			case 0x01:
				printf("数据包接收错误，回传数据 : 0x0%x\r\n",ack);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(500);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
			case 0x02:
				printf("传感器上没有手指，回传数据 : 0x0%x\r\n",ack);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(500);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
			case 0x03:
				printf("扫描图像失败，回传数据 : 0x0%x\r\n",ack);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(500);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
			case 0x08:
				printf("指纹不匹配，回传数据 : 0x0%x\r\n",ack);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(500);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
			case 0x09:
				printf("未搜索到指纹，回传数据 : 0x0%x\r\n",ack);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(500);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
			case 0x0A:
				printf("特征合并失败，回传数据 : 0x0%x\r\n",ack);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(500);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
			default :
				printf("未知错误，回传数据 : 0x0%x\r\n",ack);
				GPIO_SetBits(GPIOC,GPIO_Pin_0);
				delay_ms(500);
				GPIO_ResetBits(GPIOC,GPIO_Pin_0);
				break;
		}
		delay_ms(500);
	}
}


/**
	* @brief	USART2 串口初始化函数
	* @param 	None
	* @return None
	*/
void USART_2_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	NVIC_Config();	//中断向量，自己写
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);

	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

	USART_Cmd(USART2,ENABLE);
	
	delay_ms(200);
	
	FPM383C_Sleep(1);
}


/**
	* @brief	外部中断引脚初始化函数
	* @param 	None
	* @return None
	*/
void FPM383C_GPIO_Interrupt_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	NVIC_Config();
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource13);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC,&GPIO_InitStructure);

	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


/**
	* @brief	在main函数里调用此函数即使用全部功能
	* @param 	None
	* @return None
	*/
void FPM383C_Init(void)
{
	USART_2_Config();
	FPM383C_GPIO_Interrupt_Config();
}


/**
	* @brief	外部中断函数
	* @param 	None
	* @return None
	*/
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13);
		FPM383C_ControlLED(PS_BlueLEDBuffer);
		GPIO_SetBits(GPIOC,GPIO_Pin_0);
		delay_ms(50);
		GPIO_ResetBits(GPIOC,GPIO_Pin_0);
		ACK(FPM383C_Identify());		//验证指纹模式
		//ACK(FPM383C_Enroll());		//注册指纹模式
	}
	FPM383C_Sleep(1);
}


/**
	* @brief	串口2中断函数
	* @param 	None
	* @return None
	*/
void USART2_IRQHandler(void)
{
	uint8_t res;				
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART2);
		if((Receive_STA & (1 << 15)) == 0)
		{
			if(Receive_STA<20)
			{
				FPM383C_ReceiveBuffer[Receive_STA++]=res;
			}else{
				Receive_STA |= (1<<15);
			}
		}
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}
