#include "function.h"

FPM_HandlerTypeDef g_fpmStructure = {0};
uint8_t g_MatchScore, g_RecvFlag, g_RecvCnt, g_TrigFlag, g_RecvBuff[50]; 

//USART串口接收长度以及标志位
uint8_t USART2_STA  = 0;
uint8_t USART3_STA	= 0;

//USART串口接收缓冲数组
uint8_t USART2_ReceiveBuffer[20];
uint8_t USART3_ReceiveBuffer[6];


/**
	* @brief	微秒级延时函数
	* @param 	us：需要延时的时间，单位微秒
	* @return None
	*/
void delay_us(uint32_t us)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock / 1000000);
	for(i=0; i<us; i++)
	{
		while(!((SysTick->CTRL) & (1<<16)));
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}


/**
	* @brief	毫秒级延时函数
	* @param 	ms：需要延时的时间，单位毫秒
	* @return None
	*/
void delay_ms(uint32_t ms)
{
	uint32_t i;
	SysTick_Config(SystemCoreClock / 1000);
	for(i=0; i<ms; i++)
	{
		while(!((SysTick->CTRL) & (1<<16)));
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}


/**
	* @brief	STM32 external interrupt callback function
	* @param 	GPIO_Pin:Interrupt pin
	* @return None
	*/
void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line13);
		
		/* 关闭外部中断，防止误触发 */
		NVIC_DisableIRQ(EXTI15_10_IRQn);
		/* 触发标志位置位 */
		g_TrigFlag = 1;
	}
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
  * @brief	
  * @param 	
  * @param	
  * @return 
  */
void FPM_Send_Data(uint8_t *pData, uint8_t size)
{
	/* 标准阻塞式串口发送函数 */
	for(uint16_t i=0; i<size; i++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != RESET);
		USART_SendData(USART2, *pData++);
	}	
}


/**
  * @brief	
  * @param 	
  * @param	
  * @return 
  */
void FPM_Read_Data(uint8_t *pData)
{
	/* 开启IDLE+DMA方式接收 */
//	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, pData, 20);
}


/**
  * @brief	
  * @param 	
  * @param	
  * @return 
  */
void FPM_TrigEvenCallback(void)
{
	/* LED状态灯翻转 */
	GPIOB->ODR ^= 1<<9;
}


/**
  * @brief	
  * @param 	
  * @param	
  * @return 
  */
void FPM_Init(FPM_HandlerTypeDef *hfpm)
{
	/* 延时函数 */
	hfpm->delay = delay_ms;
	
	/* 发送接收函数 */
	hfpm->write = FPM_Send_Data;
	hfpm->read 	= FPM_Read_Data;
	
	/* 接收状态标志位及数量 */
	hfpm->pRecvCnt	= &g_RecvCnt;
	hfpm->pRecvFlag = &g_RecvFlag;
	hfpm->pRecvBuff = g_RecvBuff;
	
	/* 上电初始化后开启睡眠 */
	FPM_Sleep(hfpm);
}


/**
  * @brief	
  * @param 	
  * @param	
  * @return 
  */
void FPM_Main(FPM_HandlerTypeDef *hfpm)
{
	if(g_TrigFlag)
	{
		/* 清除触发中断标志位 */
		g_TrigFlag = 0;
		/* 显示蓝灯 */
		FPM_ControlLED(hfpm, PS_BlueLEDBuffer);
		/* 指纹采集并校验 */
		FPM_Identify(hfpm);
//		FPM_Enroll(hfpm, 0, 5000);
		/* 等待指纹模块进入睡眠 */
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0))
		{
			hfpm->delay(100);
			FPM_Sleep(hfpm);
		}
		/* 必须先清除中断标志位，再开启外部中断，否则会因为指纹模块睡眠-唤醒状态改变而再次触发中断 */
		EXTI_ClearFlag(EXTI15_10_IRQn);
		NVIC_EnableIRQ(EXTI15_10_IRQn);
	}
}
