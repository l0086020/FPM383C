/********************************************************************************************************************

v1.0因为使用的是While循环等待接收的数据，可能会因为数据接收出错或其他原因，导致程序卡死，或者将程序更改为while(--Timeout),用延时来等待接收数据。
v1.1使用IWDG独立看门狗2秒定时进行复位，这种方法可有效避免程序卡死。

中断引脚接在PC13上

在main函数里调用一下FPM383C_Init()就可以了。

********************************************************************************************************************/

#include "FPM383C.h"

//USART串口接收长度以及标志位
uint8_t USART2_STA  = 0;
uint8_t USART3_STA	= 0;

//指纹ID和验证指纹的分数
uint16_t PageID,MatchScore;

//USART串口接收缓冲数组
uint8_t USART2_ReceiveBuffer[20];
uint8_t USART3_ReceiveBuffer[6];

//主循环状态标志位
uint8_t ScanStatus = 0;

//三种模式对应的命令
char StringEnroll[7]  = "Enroll";
char StringEmpty[7]   = "Empty";
char StringDelete[7]  = "Delete";

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
	* @brief	微秒级延时函数
	* @param 	us：需要延时的时间，单位微秒
	* @return None
	*/
void delay_us(uint32_t us)
{
    uint32_t i;
    SysTick_Config(SystemCoreClock / 1000000);	//配置SysTick为10us
    for(i=0; i<us; i++)
    {
        while(!((SysTick->CTRL) & (1<<16)));	//等待计数器到0，CTRL寄存器的位16会置1，当置1时，读取该位会清0
    }
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭SysTick定时器
}


/**
	* @brief	毫秒级延时函数
	* @param 	ms：需要延时的时间，单位毫秒
	* @return None
	*/
void delay_ms(uint32_t ms)
{
    uint32_t i;
    SysTick_Config(SystemCoreClock / 1000);	//配置SysTick为1ms
    for(i=0; i<ms; i++)
    {
        while(!((SysTick->CTRL) & (1<<16)));	//等待计数器到0，CTRL寄存器的位16会置1，当置1时，读取该位会清0
    }
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//关闭SysTick定时器
}


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
		while(!USART_GetFlagStatus(USART2,USART_FLAG_TXE));
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
	while(USART2_STA<12 && (--Timeout))
	{
		delay_ms(1);
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
	while(USART2_STA<12 && (--Timeout))
	{
		delay_ms(1);
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
	while(USART2_STA<16 && (--Timeout))
	{
		delay_ms(1);
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
	while(USART2_STA<12 && (--Timeout))
	{
		delay_ms(1);
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
	while(USART2_STA<12 && (--Timeout))
	{
		delay_ms(1);
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
	while(USART2_STA<12 && (--Timeout))
	{
		delay_ms(1);
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
				MatchScore = (int)((USART2_ReceiveBuffer[10] << 8) + USART2_ReceiveBuffer[11]);
				printf("\r\n扫描成功 指纹ID：%d\r\n",(int)MatchScore);
				FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
				return;
			}
		}
	}
	printf("\r\n未搜索到指纹\r\n");
	FPM383C_ControlLED(PS_RedLEDBuffer,2000);
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
  while(USART2_STA<12 && (--Timeout))
	{
		delay_ms(1);
	}
	if(USART2_ReceiveBuffer[9] == 0x00)
	{
		printf("\r\n>>>指纹注册成功<<<\r\n");
		FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
		return;
	}
	else if(Timeout == 0)
	{
		FPM383C_SendData(12,PS_CancelBuffer);
		delay_ms(50);
	}
	FPM383C_ControlLED(PS_RedLEDBuffer,2000);
}


/**
	* @brief	中断向量函数，里面配置所需要的中断功能
	* @param 	None
	* @return None
	*/
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}


/**
	* @brief	USART3串口初始化函数
	* @param 	None
	* @return None
	*/
void USART_3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART3,&USART_InitStructure);
	
	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);
	
	USART_Cmd(USART3,ENABLE);
}


/**
	* @brief	USART2 串口初始化函数
	* @param 	None
	* @return None
	*/
void USART_2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
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
	* @brief	外部中断服务函数
	* @param 	None
	* @return None
	*/
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		ScanStatus |= 1<<7;
		EXTI_ClearITPendingBit(EXTI_Line13);
	}
}


/**
  * @brief  串口3中断服务函数
  * @param  None
  * @return None
  */
void USART3_IRQHandler(void)
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)
	{
		uint8_t res = USART_ReceiveData(USART3);
		if((USART3_STA & 0x80) == 0)
		{
			if(USART3_STA < 6)
			{
				USART3_ReceiveBuffer[USART3_STA++] = res;
			}
			else
			{
				USART3_STA |= 0x80;
			}
		}
	}
	USART_ClearITPendingBit(USART3,USART_IT_RXNE);
}


/**
	* @brief	串口2中断服务函数
	* @param 	None
	* @return None
	*/
void USART2_IRQHandler(void)
{
	uint8_t res;				
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		res = USART_ReceiveData(USART2);
		if((USART2_STA & 0x80) == 0)
		{
			if(USART2_STA < 20)
			{
				USART2_ReceiveBuffer[USART2_STA++] = res;
			}
			else
			{
				USART2_STA |= 0x80;
			}
		}
	}
	USART_ClearITPendingBit(USART2,USART_IT_RXNE);
}


/**
	* @brief	printf重映射函数，重映射后可以直接使用printf函数，并直接打印到USART3串口上
	* @param 	unknow
	* @return unknow
	*/
int fputc(int ch,FILE *f)
{
	USART_SendData(USART3,(uint8_t)ch);
	while(!USART_GetFlagStatus(USART3,USART_FLAG_TXE));
	return ch;
}


/**
	* @brief	在main函数里调用此函数以初始化FPM383C指纹模块
	* @param 	None
	* @return None
	*/
void FPM383C_Init(void)
{
	NVIC_Config();
	USART_2_Config();
	USART_3_Config();
	FPM383C_Sleep();
	FPM383C_GPIO_Interrupt_Config();
}


/**
  * @brief  在main函数的While(1)循环里调用此函数以使用FPM383C的大部分功能
  * @param  None
  * @return None
  */
void FPM383C_Loop(void)
{
	if(strstr((char *)USART3_ReceiveBuffer,(char *)StringEnroll) != NULL)
	{
		printf("\r\n————注册指纹模式————\r\n");
		
		/* 清除之前数据的残留，为下一次接收数据准备 */
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0xFF,sizeof(USART3_ReceiveBuffer));

		printf("\r\n>>>请先输入指纹ID<<<\r\n");
		
		/* 等待接收标志位有数据置位 */
		while(USART3_STA <= 0);
		delay_ms(2);
		
		/* 使用atoi(const char * string)将输入的指纹ID号从字符串数组转换成整形数字 */
		PageID = atoi((char *)USART3_ReceiveBuffer);
		printf("\r\n>>>输入的指纹ID：%d<<<\r\n",PageID);
		
		/* 清除上一个接收的标志位以及清空接收缓冲数组 */
		USART3_STA = 0;		
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
		
		/* 将状态标志位置位，传给下面的Switch函数调用对应的功能 */
		ScanStatus |= 1<<3;
		printf("\r\n>>>请将需要注册的手指按压指纹模块4次<<<\r\n");
	}
	else if(strstr((char *)USART3_ReceiveBuffer,(char *)StringDelete) != NULL)
	{
		printf("\r\n————删除指纹模式————\r\n");
		
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
		
		printf("\r\n>>>请先输入指纹ID<<<\r\n");
		
		while(USART3_STA <= 0);
		
		delay_ms(2);
		
		PageID = atoi((char *)USART3_ReceiveBuffer);
		printf("\r\n>>>输入的指纹ID：%d<<<\r\n",PageID);
		
		if(FPM383C_Delete(PageID,2000) == 0x00)
		{
			FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
			printf("\r\n>>>已删除指纹<<<\r\n");
		}
		
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
	}
	else if(strstr((char *)USART3_ReceiveBuffer,(char *)StringEmpty) != NULL)
	{
		printf("\r\n————清空指纹模式————\r\n");
		if(FPM383C_Empty(2000) == 0x00)
		{
			FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
			printf("\r\n　　　>>>已清空指纹<<<　　　\r\n");
		}
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
	}
	
	switch(ScanStatus)
	{
		case 0x80:
			FPM383C_ControlLED(PS_BlueLEDBuffer,2000);
			delay_ms(5);
			FPM383C_Identify();						//验证指纹模式
			delay_ms(500);
			FPM383C_Sleep();
			ScanStatus = 0;
		break;
		
		case 0x88:
			FPM383C_ControlLED(PS_BlueLEDBuffer,2000);
			delay_ms(5);
			FPM383C_Enroll(PageID,10000);		//注册指纹模式
			delay_ms(500);
			FPM383C_Sleep();
			ScanStatus = 0;
		break;
	}
}
