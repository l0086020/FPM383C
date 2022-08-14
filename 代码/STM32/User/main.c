#include "stm32f10x.h"
#include "FPM383C.h"

/**
	* @Date					2022/08/14
	* @Version			v1.1.0
	* @Description	注册指纹与搜索指纹功能都写在了中断函数里，所以在main函数里只需要初始化一下即可，不需要进行其他操作。中断函数写在FPM383C.c第489行
	*/
	
int main()
{
	FPM383C_Init();	//指纹模块初始化函数
	while(1)
	{
		
	}
}
