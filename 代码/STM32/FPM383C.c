/********************************************************************************************************************

v1.0因为使用的是While循环等待接收的数据，可能会因为数据接收出错或其他原因，导致程序卡死，或者将程序更改为while(--Timeout),用延时来等待接收数据。
v1.1使用IWDG独立看门狗2秒定时进行复位，这种方法可有效避免程序卡死。

中断引脚接在PC13上

在main函数里调用一下FPM383C_Init()就可以了。

********************************************************************************************************************/

#include "stdio.h"
#include "stm32f10x.h"

uint8_t Receive_STA = 0;
uint16_t PageID,MatchScore;
uint8_t FPM383C_ReceiveBuffer[20];

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

	
/**
	* @brief	微秒级延时函数
	* @param 	us：需要延时的时间，单位微秒
	* @return None
	*/
void delay_us(uint32_t us)
{
    uint32_t i;
    SysTick_Config(SystemCoreClock/1000000);	//配置SysTick为10us
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
    SysTick_Config(72000);	//配置SysTick为1ms
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
	* @brief	清空指纹库
	* @param 	Timeout：接收数据的超时时间
	* @return 确认码
	*/
uint8_t FPM383C_Empty(uint32_t Timeout)
{
	FPM383C_SendData(12,PS_EmptyBuffer);
	while(Receive_STA<12 && (--Timeout))
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
	* @brief	注册指纹函数
	* @param 	PageID：输入需要注册的指纹ID号，取值范围0—59
	* @param	Timeout：设置注册指纹超时时间，因为需要按压四次手指，建议大于10000（即10s）
	* @return 确认码or无效码
	*/
uint8_t FPM383C_Enroll(uint16_t PageID,uint16_t Timeout)
{
	PS_AutoEnrollBuffer[10] = (PageID>>8);
  PS_AutoEnrollBuffer[11] = (PageID);
  PS_AutoEnrollBuffer[15] = (0x54+PS_AutoEnrollBuffer[10]+PS_AutoEnrollBuffer[11])>>8;
  PS_AutoEnrollBuffer[16] = (0x54+PS_AutoEnrollBuffer[10]+PS_AutoEnrollBuffer[11]);
  FPM383C_SendData(17,PS_AutoEnrollBuffer);
  while(Receive_STA<12 && (--Timeout))
	{
		delay_ms(1);
	}
	if(FPM383C_ReceiveBuffer[9] == 0x00)
	{
		FPM383C_ControlLED(PS_GreenLEDBuffer);
		return FPM383C_ReceiveBuffer[9];
	}
	else if(Timeout == 0)
	{
		FPM383C_SendData(12,PS_CancelBuffer);
		delay_ms(50);
	}
	FPM383C_ControlLED(PS_RedLEDBuffer);
	return 0xFF;
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
	* @brief	中断向量函数，里面配置所需要的中断功能
	* @param 	None
	* @return None
	*/
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_SetPriorityGrouping(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
}

/**
	* @brief	USART1串口初始化函数（可要可不要，考虑到可能需要Debug，所以先初始化一遍）
	* @param 	None
	* @return None
	*/
void USART_1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1,&USART_InitStructure);
	
	USART_Cmd(USART1,ENABLE);
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
	
	NVIC_Config();
	
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
	USART_1_Config();
	USART_2_Config();
	FPM383C_GPIO_Interrupt_Config();
}

/**
	* @brief	printf重映射函数，重映射后可以直接使用printf函数，并直接打印到USART1串口上
	* @param 	unknow
	* @return unknow
	*/
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(uint8_t)ch);
	while(!USART_GetFlagStatus(USART1,USART_FLAG_TXE));
	return ch;
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
		//ACK(FPM383C_Identify());		//验证指纹模式
		ACK(FPM383C_Enroll(0,10000));		//注册指纹模式
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
