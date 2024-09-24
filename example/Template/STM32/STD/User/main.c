/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * 此工程为STM32F1x系列的标准库，使用Keil Arm V6编译器。若使用V5编译器，需将
	* core_cm3.c添加到工程目录中，具体文件在./CMSIS/core_cm3.c
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
	/* 开机延时 */
	delay_ms(1000);
	
	/* 中断向量初始化 */
	STM32_NVIC_Init();
	
	/* 串口参数初始化 */
	STM32_USART2_Init();
	
	/* 外部中断线初始化 */
	STM32_EXTI_Init();
	
	/* GPIO引脚初始化 */
	STM32_GPIO_Init();
	
	/* 指纹模块初始化 */
	FPM_Init(&g_fpmStructure);
	
	while(1)
	{
		/* 指纹模块功能示例 */
		FPM_Main(&g_fpmStructure);
	}
}
