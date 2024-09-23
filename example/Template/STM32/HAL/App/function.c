#include "function.h"

uint8_t g_MatchScore, g_RecvFlag, g_RecvCnt, g_TrigFlag, g_RecvBuff[50]; 

/**
  * @brief	
  * @param 	
  * @param	
  * @return 
  */
void FPM_Send_Data(uint8_t *pData, uint8_t size)
{
	/* HAL库标准阻塞式串口发送函数 */
	HAL_UART_Transmit(&huart2, pData, size, 0x100);
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
		do
		{
			FPM_Sleep(hfpm);
			HAL_Delay(100);
		}
		while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0));
		
		/* 必须先清除中断标志位，再开启外部中断，否则会因为指纹模块睡眠-唤醒状态改变而再次触发中断 */
		__HAL_GPIO_EXTI_CLEAR_IT(TRIG_PIN_Pin);
		HAL_NVIC_EnableIRQ(TRIG_PIN_EXTI_IRQn);
	}
}
