#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "main.h"
#include "stdbool.h"
#include "fpm_module.h"

extern FPM_HandlerTypeDef g_fpmStructure;
extern uint8_t g_MatchScore, g_RecvFlag, g_RecvCnt, g_TrigFlag, g_RecvBuff[50]; 

/*  */
void delay_ms(uint32_t ms);
void delay_us(uint32_t us);

/*  */
void FPM_Send_Data(uint8_t *pData, uint8_t size);
void FPM_Read_Data(uint8_t *pData);

/*  */
void FPM_Main(FPM_HandlerTypeDef *hfpm);

#endif
