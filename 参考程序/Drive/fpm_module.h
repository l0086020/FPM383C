#ifndef __FPM_MODULE_H
#define __FPM_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdio.h"
#include "string.h"


/* ָ��ģ�����ýṹ�� */
typedef void (*FPM_Read) (uint8_t *);						/* ��ָ���ԭ�ͣ��β�Ϊ�������ݻ������� */
typedef void (*FPM_Write)(uint8_t *, uint8_t);	/* дָ���ԭ�ͣ��β�Ϊ�������ݻ������顢Size��С */
typedef void (*FPM_Delay)(uint32_t);						/* ��ʱ����ԭ�ͣ����ݲ�ͬƽ̨�����ݲ�ͬ��ʱ���� */
typedef struct
{
	/* ָ����ջ���������־λ����С��ָ�� */
	uint8_t *	 pRecvFlag;		// ����״̬��־λ
	uint8_t *  pRecvCnt;		// ��������
	uint8_t *  pRecvBuff;		// ���ջ�������
	
	/* ָ��ƽ̨���͡����ա���ʱ�ĺ���ָ�� */
	FPM_Delay	 delay;				// ��ʱ����
	FPM_Read	 read;				// ��ָ���
	FPM_Write	 write;				// дָ���
	
}	FPM_HandlerTypeDef;


/* FPMָ���ȷ����Ƭѡ */
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


/* ָ��ģ���ʼ������ */
void FPM_Init(FPM_HandlerTypeDef *hfpm);
/* ָ��ģ��˯�ߺ��� */
uint8_t FPM_Sleep(FPM_HandlerTypeDef *hfpm);
/* LED���ƺ��� */
uint8_t FPM_ControlLED(FPM_HandlerTypeDef *hfpm, uint8_t *pColorBuff);
/* ע��ָ�ƺ��� */
uint8_t FPM_Enroll(FPM_HandlerTypeDef *hfpm, uint16_t g_PageID, uint16_t Timeout);
/* ���ָ�ƺ��� */
uint8_t FPM_Empty(FPM_HandlerTypeDef *hfpm);
/* ��ָ֤�ƺ��� */
uint16_t FPM_Identify(FPM_HandlerTypeDef *hfpm);

#endif
