#include "main.h"

extern FPM_HandlerTypeDef g_fpmStructure;

int main(void)
{
	STM32_NVIC_Config();
	USART_2_Config();
	USART_3_Config();
	GPIO_Interrupt_Config();
	FPM_Init(&g_fpmStructure);
	
	while(1)
	{
		FPM_Main(&g_fpmStructure);
	}
}
