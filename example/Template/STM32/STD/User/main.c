#include "main.h"

/* 需使用Keil的V5编译器，另外程序还没有完全移植过来，先不要使用STD库 */

void FPM_Main(FPM_HandlerTypeDef *hfpm);
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
