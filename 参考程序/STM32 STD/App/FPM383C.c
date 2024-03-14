/********************************************************************************************************************

v1.0��Ϊʹ�õ���Whileѭ���ȴ����յ����ݣ����ܻ���Ϊ���ݽ��ճ��������ԭ�򣬵��³����������߽��������Ϊwhile(--Timeout),����ʱ���ȴ��������ݡ�
v1.1ʹ��IWDG�������Ź�2�붨ʱ���и�λ�����ַ�������Ч�����������

�ж����Ž���PC13��

��main���������һ��FPM383C_Init()�Ϳ����ˡ�

********************************************************************************************************************/

#include "FPM383C.h"

//USART���ڽ��ճ����Լ���־λ
uint8_t USART2_STA  = 0;
uint8_t USART3_STA	= 0;

//ָ��ID����ָ֤�Ƶķ���
uint16_t PageID,MatchScore;

//USART���ڽ��ջ�������
uint8_t USART2_ReceiveBuffer[20];
uint8_t USART3_ReceiveBuffer[6];

//��ѭ��״̬��־λ
uint8_t ScanStatus = 0;

//����ģʽ��Ӧ������
char StringEnroll[7]  = "Enroll";
char StringEmpty[7]   = "Empty";
char StringDelete[7]  = "Delete";

//ģ��LED�ƿ���Э��
uint8_t PS_BlueLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x03,0x01,0x01,0x00,0x00,0x49};
uint8_t PS_RedLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x04,0x04,0x02,0x00,0x50};
uint8_t PS_GreenLEDBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x02,0x02,0x02,0x00,0x4C};

//ģ��˯��Э��
uint8_t PS_SleepBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x33,0x00,0x37};

//���ָ��Э��
uint8_t PS_EmptyBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};

//ȡ������Э��
uint8_t PS_CancelBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x30,0x00,0x34};

//�Զ�ע��ָ��Э��
uint8_t PS_AutoEnrollBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x31,'\0','\0',0x04,0x00,0x16,'\0','\0'}; //PageID: bit 10:11��SUM: bit 15:16

//ģ�������û�ȡͼ��Э��
uint8_t PS_GetImageBuffer[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};

//ģ������ģ��Э��
uint8_t PS_GetChar1Buffer[13] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};
uint8_t PS_GetChar2Buffer[13] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};

//ģ������ָ�ƹ���Э��
uint8_t PS_SearchMBBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0xFF,0xFF,0x02,0x0C};

//ɾ��ָ��Э��
uint8_t PS_DeleteBuffer[16] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,'\0','\0',0x00,0x01,'\0','\0'}; //PageID: bit 10:11��SUM: bit 14:15

	
/**
	* @brief	΢�뼶��ʱ����
	* @param 	us����Ҫ��ʱ��ʱ�䣬��λ΢��
	* @return None
	*/
void delay_us(uint32_t us)
{
    uint32_t i;
    SysTick_Config(SystemCoreClock / 1000000);	//����SysTickΪ10us
    for(i=0; i<us; i++)
    {
        while(!((SysTick->CTRL) & (1<<16)));	//�ȴ���������0��CTRL�Ĵ�����λ16����1������1ʱ����ȡ��λ����0
    }
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�ر�SysTick��ʱ��
}


/**
	* @brief	���뼶��ʱ����
	* @param 	ms����Ҫ��ʱ��ʱ�䣬��λ����
	* @return None
	*/
void delay_ms(uint32_t ms)
{
    uint32_t i;
    SysTick_Config(SystemCoreClock / 1000);	//����SysTickΪ1ms
    for(i=0; i<ms; i++)
    {
        while(!((SysTick->CTRL) & (1<<16)));	//�ȴ���������0��CTRL�Ĵ�����λ16����1������1ʱ����ȡ��λ����0
    }
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	//�ر�SysTick��ʱ��
}


/**
	* @brief	USART2���ڷ��ͺ���
	* @param 	length: �������鳤��
	* @param	FPM383C_Databuffer[]: ��Ҫ���͵Ĺ���Э�����飬���������ж���
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
	* @brief	����˯��ָ�ֻ�з���˯��ָ���TOUCHOUT�Ż��ɵ͵�ƽ
	* @param 	None
	* @return None
	*/
void FPM383C_Sleep(void)
{
	FPM383C_SendData(12,PS_SleepBuffer);
}


/**
	* @brief	���ͻ�ȡָ��ͼ��ָ��
	* @param 	Timeout���������ݵĳ�ʱʱ��
	* @return ȷ����
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
	* @brief	��������ģ��ָ��
	* @param 	Timeout���������ݵĳ�ʱʱ��
	* @return ȷ����
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
	* @brief	��������ָ��ָ��
	* @param 	Timeout���������ݵĳ�ʱʱ��
	* @return ȷ����
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
	* @brief	ɾ��ָ��ָ��ָ��
  * @param  PageID ����Ҫɾ����ָ��ID��
	* @param 	Timeout���������ݵĳ�ʱʱ��
	* @return ȷ����
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
	* @brief	���ָ�ƿ�
	* @param 	Timeout���������ݵĳ�ʱʱ��
	* @return ȷ����
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
	* @brief	���Ϳ��Ƶƹ�ָ��
	* @param 	PS_ControlLEDBuffer[]����ͬ��ɫ��Э������
  * @param  Timeout���������ݵĳ�ʱʱ��
	* @return ȷ����
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
	* @brief	��ָ֤������
	* @param 	None
	* @return ȷ����
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
				printf("\r\nɨ��ɹ� ָ��ID��%d\r\n",(int)MatchScore);
				FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
				return;
			}
		}
	}
	printf("\r\nδ������ָ��\r\n");
	FPM383C_ControlLED(PS_RedLEDBuffer,2000);
}

/**
	* @brief	ע��ָ�ƺ���
	* @param 	PageID��������Ҫע���ָ��ID�ţ�ȡֵ��Χ0��59
	* @param	Timeout������ע��ָ�Ƴ�ʱʱ�䣬��Ϊ��Ҫ��ѹ�Ĵ���ָ���������10000����10s��
	* @return ȷ����
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
		printf("\r\n>>>ָ��ע��ɹ�<<<\r\n");
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
	* @brief	�ж�����������������������Ҫ���жϹ���
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
	* @brief	USART3���ڳ�ʼ������
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
	* @brief	USART2 ���ڳ�ʼ������
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
	* @brief	�ⲿ�ж����ų�ʼ������
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
	* @brief	�ⲿ�жϷ�����
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
  * @brief  ����3�жϷ�����
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
	* @brief	����2�жϷ�����
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
	* @brief	printf��ӳ�亯������ӳ������ֱ��ʹ��printf��������ֱ�Ӵ�ӡ��USART3������
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
	* @brief	��main��������ô˺����Գ�ʼ��FPM383Cָ��ģ��
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
  * @brief  ��main������While(1)ѭ������ô˺�����ʹ��FPM383C�Ĵ󲿷ֹ���
  * @param  None
  * @return None
  */
void FPM383C_Loop(void)
{
	if(strstr((char *)USART3_ReceiveBuffer,(char *)StringEnroll) != NULL)
	{
		printf("\r\n��������ע��ָ��ģʽ��������\r\n");
		
		/* ���֮ǰ���ݵĲ�����Ϊ��һ�ν�������׼�� */
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0xFF,sizeof(USART3_ReceiveBuffer));

		printf("\r\n>>>��������ָ��ID<<<\r\n");
		
		/* �ȴ����ձ�־λ��������λ */
		while(USART3_STA <= 0);
		delay_ms(2);
		
		/* ʹ��atoi(const char * string)�������ָ��ID�Ŵ��ַ�������ת������������ */
		PageID = atoi((char *)USART3_ReceiveBuffer);
		printf("\r\n>>>�����ָ��ID��%d<<<\r\n",PageID);
		
		/* �����һ�����յı�־λ�Լ���ս��ջ������� */
		USART3_STA = 0;		
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
		
		/* ��״̬��־λ��λ�����������Switch�������ö�Ӧ�Ĺ��� */
		ScanStatus |= 1<<3;
		printf("\r\n>>>�뽫��Ҫע�����ָ��ѹָ��ģ��4��<<<\r\n");
	}
	else if(strstr((char *)USART3_ReceiveBuffer,(char *)StringDelete) != NULL)
	{
		printf("\r\n��������ɾ��ָ��ģʽ��������\r\n");
		
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
		
		printf("\r\n>>>��������ָ��ID<<<\r\n");
		
		while(USART3_STA <= 0);
		
		delay_ms(2);
		
		PageID = atoi((char *)USART3_ReceiveBuffer);
		printf("\r\n>>>�����ָ��ID��%d<<<\r\n",PageID);
		
		if(FPM383C_Delete(PageID,2000) == 0x00)
		{
			FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
			printf("\r\n>>>��ɾ��ָ��<<<\r\n");
		}
		
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
	}
	else if(strstr((char *)USART3_ReceiveBuffer,(char *)StringEmpty) != NULL)
	{
		printf("\r\n�����������ָ��ģʽ��������\r\n");
		if(FPM383C_Empty(2000) == 0x00)
		{
			FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
			printf("\r\n������>>>�����ָ��<<<������\r\n");
		}
		USART3_STA = 0;
		memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
	}
	
	switch(ScanStatus)
	{
		case 0x80:
			FPM383C_ControlLED(PS_BlueLEDBuffer,2000);
			delay_ms(5);
			FPM383C_Identify();						//��ָ֤��ģʽ
			delay_ms(500);
			FPM383C_Sleep();
			ScanStatus = 0;
		break;
		
		case 0x88:
			FPM383C_ControlLED(PS_BlueLEDBuffer,2000);
			delay_ms(5);
			FPM383C_Enroll(PageID,10000);		//ע��ָ��ģʽ
			delay_ms(500);
			FPM383C_Sleep();
			ScanStatus = 0;
		break;
	}
}
