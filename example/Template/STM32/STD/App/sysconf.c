#include "sysconf.h"
#include "function.h"

/**
	* @brief	中断向量配置函数
	* @param 	None
	* @return None
	*/
void STM32_NVIC_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* 中断向量分组 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* 外部中断向量配置 */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 串口中断向量配置 */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  GPIO引脚初始化函数
  * @param  None
  * @retval None
  */
void STM32_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
	

/**
	* @brief	USART2 串口初始化函数
	* @param 	None
	* @return None
	*/
void STM32_USART2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
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
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}


/**
	* @brief	外部中断引脚初始化函数
	* @param 	None
	* @return None
	*/
void STM32_EXTI_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}


/**
	* @brief	STM32 external interrupt callback function
	* @param 	GPIO_Pin:Interrupt pin
	* @return None
	*/
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		/* 清除中断标志位 */
		EXTI_ClearITPendingBit(EXTI_Line0);
		/* 关闭外部中断，防止误触发 */
		NVIC_DisableIRQ(EXTI0_IRQn);
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
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		g_RecvBuff[g_RecvCnt++] = USART2->DR;
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	}
	else if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)
	{
		USART_ReceiveData(USART2);
		g_RecvFlag = 1;
	}
}
