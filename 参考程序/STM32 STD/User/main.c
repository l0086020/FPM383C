#include "stm32f10x.h"
#include "FPM383C.h"

int main()
{
	FPM383C_Init();	//ָ��ģ���ʼ������
	while(1)
	{
		FPM383C_Loop();
	}
}
