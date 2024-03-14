#include "stm32f10x.h"
#include "FPM383C.h"

int main()
{
	FPM383C_Init();	//指纹模块初始化函数
	while(1)
	{
		FPM383C_Loop();
	}
}
