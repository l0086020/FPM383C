#include "function.h"

uint8_t g_MatchScore, g_RecvFlag, g_RecvCnt, g_TrigFlag, g_RecvBuff[50]; 

/**
  * @brief	用于FPM模块的串口发送函数，请根据函数原型定义该函数
  * @param 	None
  * @return None
  */
void FPM_Send_Data(uint8_t *pData, uint8_t size)
{
	/* HAL库标准阻塞式串口发送函数 */
	HAL_UART_Transmit(&huart2, pData, size, 0x100);
}


/**
  * @brief	用于FPM模块的串口接收函数，不管如何操作串口，该函数都需定义，函数体可为空
  * @param 	None
  * @return None
  */
void FPM_Read_Data(uint8_t *pData)
{
	/* 开启IDLE+DMA方式接收 */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, pData, 20);
}


/**
  * @brief	
  * @param 	
  * @param	
  * @return 
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART2)
	{
		/* 接收完成标志位 */
		g_RecvFlag = 1;
		/* 接收数据大小变量 */
		g_RecvCnt = Size;
	}
}


/**
	* @brief	STM32 external interrupt callback function
	* @param 	GPIO_Pin:Interrupt pin
	* @return None
	*/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == TRIG_PIN_Pin)
	{
		/* 关闭外部中断，防止误触发 */
		HAL_NVIC_DisableIRQ(TRIG_PIN_EXTI_IRQn);
		/* 触发标志位置位 */
		g_TrigFlag = 1;
	}
}


/**
  * @brief	指纹验证成功回调函数。简单功能可直接实现，复杂功能需操作标志位，另写实现函数
  * @param 	MatchID：返回的指纹ID信息
  * @return None
  */
void FPM_SuccessEventCallback(uint16_t MatchID)
{
	if(MatchID == 0)
		GPIOB->ODR ^= 1<<1;			/* LED状态灯翻转 */
	else if(MatchID == 1)
		GPIOB->ODR |= 1<<1;			/* LED状态灯关闭 */
	else if(MatchID == 2)
		GPIOB->ODR &= ~(1<<1);	/* LED状态灯打开 */
}


/**
  * @brief	FPM模块初始化函数，用于指定所需的延时函数、串口发送接收函数以及缓冲区及标志位等
  * @param 	hfpm：操作指纹模块的句柄结构体
  * @return None
  */
void FPM_Init(FPM_HandlerTypeDef *hfpm)
{
	/* 延时函数 */
	hfpm->delay = HAL_Delay;
	
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
  * @brief	FPM模块主函数，函数体内的do...while是为了让手指长时接触模块松开后可靠进入休眠模式，建议加上，同时下方的中断标志位也如此
  * @param 	hfpm：操作指纹模块的句柄结构体
  * @return None
  */
void FPM_Main(FPM_HandlerTypeDef *hfpm)
{
	if(g_TrigFlag)
	{
		/* 清除触发中断标志位 */
		g_TrigFlag = 0;

		/* 指纹采集并校验 */
		FPM_Identify(hfpm);
//		FPM_Enroll(hfpm, 0, 5000);
		
		/* 等待指纹模块进入睡眠 */
		do
		{
			FPM_Sleep(hfpm);
			HAL_Delay(100);
		}
		while(HAL_GPIO_ReadPin(TRIG_PIN_GPIO_Port, TRIG_PIN_Pin));
		
		/* 必须先清除中断标志位，再开启外部中断，否则会因为指纹模块睡眠-唤醒状态改变而再次触发中断 */
		__HAL_GPIO_EXTI_CLEAR_IT(TRIG_PIN_Pin);
		HAL_NVIC_EnableIRQ(TRIG_PIN_EXTI_IRQn);
	}
}
