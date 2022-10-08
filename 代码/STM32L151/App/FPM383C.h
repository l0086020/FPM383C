#ifndef __FPM383C_H
#define __FPM383C_H

#include "stdio.h"
#include "stdlib.h"
#include "String.h"
#include "main.h"
#include "usart.h"

void FPM383C_Init(void);

void FPM383C_SendData(int length,uint8_t FPM383C_Databuffer[]);

void FPM383C_Sleep(void);

uint8_t FPM383C_GetImage(uint32_t Timeout);
uint8_t FPM383C_GenChar1(uint32_t Timeout);
uint8_t FPM383C_GenChar2(uint32_t Timeout);
uint8_t FPM383C_SearchMB(uint32_t Timeout);
uint8_t FPM383C_Empty(uint32_t Timeout);
uint8_t FPM383C_Delete(uint16_t PageID,uint32_t Timeout);
uint8_t FPM383C_ControlLED(uint8_t PS_ControlLEDBuffer[],uint32_t Timeout);

void FPM383C_Identify(void);
void FPM383C_Enroll(uint16_t PageID,uint16_t Timeout);

void ACK(uint8_t ack);

#endif
