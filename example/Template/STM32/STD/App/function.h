#ifndef __FUNCTION_H
#define __FUNCTION_H

#include "main.h"

extern uint8_t g_MatchScore, g_RecvFlag, g_RecvCnt, g_TrigFlag; 


void FPM_Send_Data(uint8_t *pData, uint8_t size);
void FPM_Read_Data(uint8_t *pData);

#endif