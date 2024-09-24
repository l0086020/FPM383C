/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * �˹���ΪSTM32F1xϵ�еı�׼�⣬ʹ��Keil Arm V6����������ʹ��V5���������轫
	* core_cm3.c��ӵ�����Ŀ¼�У������ļ���./CMSIS/core_cm3.c
  ******************************************************************************
  */
#include "main.h"
#include "sysconf.h"
#include "function.h"
#include "fpm_module.h"

/**
  * @brief  
  * @param  
  * @retval 
  */
int main(void)
{
	/* ������ʱ */
	delay_ms(1000);
	
	/* �ж�������ʼ�� */
	STM32_NVIC_Init();
	
	/* ���ڲ�����ʼ�� */
	STM32_USART2_Init();
	
	/* �ⲿ�ж��߳�ʼ�� */
	STM32_EXTI_Init();
	
	/* GPIO���ų�ʼ�� */
	STM32_GPIO_Init();
	
	/* ָ��ģ���ʼ�� */
	FPM_Init(&g_fpmStructure);
	
	while(1)
	{
		/* ָ��ģ�鹦��ʾ�� */
		FPM_Main(&g_fpmStructure);
	}
}
