#ifndef __FPM_MODULE_H
#define __FPM_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdio.h"
#include "string.h"


/* 指纹模块配置结构体 */
typedef void (*FPM_Read) (uint8_t *);						/* 读指令函数原型，形参为接收数据缓冲数组 */
typedef void (*FPM_Write)(uint8_t *, uint8_t);	/* 写指令函数原型，形参为发送数据缓冲数组、Size大小 */
typedef void (*FPM_Delay)(uint32_t);						/* 延时函数原型，依据不同平台，传递不同延时函数 */
typedef struct
{
	/* 指向接收缓冲区、标志位、大小的指针 */
	uint8_t *	 pRecvFlag;		// 接收状态标志位
	uint8_t *  pRecvCnt;		// 接收数量
	uint8_t *  pRecvBuff;		// 接收缓冲数组
	
	/* 指向平台发送、接收、延时的函数指针 */
	FPM_Delay	 delay;				// 延时函数
	FPM_Read	 read;				// 读指令函数
	FPM_Write	 write;				// 写指令函数
	
}	FPM_HandlerTypeDef;


/* FPM指令返回确认码片选 */
typedef enum
{
	FPM_OK				=	0x00,
	FPM_ERR				= 0x01,
	FPM_NOFINGER	= 0x02,
	FPM_UNMATCHED	= 0x08,
	FPM_NOTFOUND	= 0xFF,
	
} FPM_StatusTypeDef;


/* External varibbels */
extern uint8_t PS_SleepBuffer[12];
extern uint8_t PS_EmptyBuffer[12];			
extern uint8_t PS_CancelBuffer[12];
extern uint8_t PS_BlueLEDBuffer[16];
extern uint8_t PS_RedLEDBuffer[16];
extern uint8_t PS_GreenLEDBuffer[16];
extern uint8_t PS_AutoEnrollBuffer[17];
extern uint8_t PS_GetImageBuffer[12];
extern uint8_t PS_GetChar1Buffer[13];
extern uint8_t PS_GetChar2Buffer[13];	
extern uint8_t PS_SearchMBBuffer[17];
extern uint8_t PS_DeleteBuffer[16];


/* 指纹模块初始化函数 */
void FPM_Init(FPM_HandlerTypeDef *hfpm);
/* 指纹模块睡眠函数 */
uint8_t FPM_Sleep(FPM_HandlerTypeDef *hfpm);
/* LED控制函数 */
uint8_t FPM_ControlLED(FPM_HandlerTypeDef *hfpm, uint8_t *pColorBuff);
/* 注册指纹函数 */
uint8_t FPM_Enroll(FPM_HandlerTypeDef *hfpm, uint16_t g_PageID, uint16_t Timeout);
/* 清空指纹函数 */
uint8_t FPM_Empty(FPM_HandlerTypeDef *hfpm);
/* 验证指纹函数 */
uint16_t FPM_Identify(FPM_HandlerTypeDef *hfpm);

#endif
