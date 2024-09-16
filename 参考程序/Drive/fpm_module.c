/* Description ---------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "fpm_module.h"
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* ˯��ָ�� */
uint8_t PS_SleepBuffer[12] 			= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x33,0x00,0x37};
/* ���ָ��ָ�� */
uint8_t PS_EmptyBuffer[12] 			= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};
/* ȡ��ָ�� */
uint8_t PS_CancelBuffer[12] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x30,0x00,0x34};
/* ��ɫLED�ƿ���ָ�� */
uint8_t PS_BlueLEDBuffer[16] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x03,0x01,0x01,0x00,0x00,0x49};
/* ��ɫLED��˫��ָ�� */
uint8_t PS_RedLEDBuffer[16] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x04,0x04,0x02,0x00,0x50};
/* ��ɫLED��˫��ָ�� */
uint8_t PS_GreenLEDBuffer[16] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x02,0x02,0x02,0x00,0x4C};
/* �Զ�ע��ָ��ָ�� */
uint8_t PS_AutoEnrollBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x31,'\0','\0',0x04,0x00,0x0F,'\0','\0'};
/* ��ȡͼ��ָ�� */
uint8_t PS_GetImageBuffer[12] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};
/* ��������1ָ�� */
uint8_t PS_GetChar1Buffer[13] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};
/* ��������2ָ�� */
uint8_t PS_GetChar2Buffer[13] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};
/* ����ģ��ָ�� */
uint8_t PS_SearchMBBuffer[17] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0xFF,0xFF,0x02,0x0C};
/* ɾ��ģ��ָ�� */
uint8_t PS_DeleteBuffer[16] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,'\0','\0',0x00,0x01,'\0','\0'};
/* ��ȡָ��ΨһSN��ָ�� */
uint8_t PS_GetChipSN[13] 				= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x34,0x00,0x00,0x39};
/* ����ָ�� */
uint8_t PS_HandShake[12] 				= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x35,0x00,0x39};
/* ��ȡ��Чģ������ָ�� */
uint8_t PS_ValidTemplateNum[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1D,0x00,0x21};
/* Global variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief	ָ����֤�ɹ����û��߼�����ʵ�֣��ú��������壬�����ⲿ��д
  * @param 	None
  * @return None
  */
__weak void FPM_TrigEvenCallback(void)
{
	/* UNUSED */
}


/**
  * @brief	ָ��ģ���ʼ���������ú��������壬�����ⲿ��д
  * @param 	hfpm��ָ��ģ����
  * @return none
  */
__weak void FPM_Init(FPM_HandlerTypeDef *hfpm)
{
	/* USER CODE FOR INITIALIZATION TO STRUCTURE -------------------------------*/
	
	/* Please improve the following code ****
		
		* ��ʱ����
		hfpm->delay = ****;
		
		* ���ͽ��պ���
		hfpm->write = ****;
		hfpm->read 	= ****;
		
		* ����״̬��־λ����С����������
		hfpm->pRecvCnt	= &****;
		hfpm->pRecvFlag = &****;
		hfpm->pRecvBuff = ****;
	
		* �ϵ��ʼ������˯��
		FPM_Sleep(hfpm);
	
	*/
}


/**
  * @brief	�ڲ�ָ��У��ͺ���
  * @param 	pBuff����ҪУ�������
  * @param	size�����ݴ�С
  * @return sum��У���
  */
static uint16_t FPM_Cmd_CheckSum(uint8_t *pBuff, uint8_t size)
{
	uint16_t sum = 0;
	uint8_t Size = size - 8;
	uint8_t *pData = pBuff + 6;
	
	/* ����У��� */
	for(uint8_t i=0; i<Size; i++)
	{
		sum += *(pData+i);
	}
	return sum;
}


/**
  * @brief  ָ��ģ���ָ����շ�����
  * @param	hfpm��ָ��ģ����
  * @param 	pBuff��ָ���跢��ָ���ָ��
  * @param	size�����ݴ�С
  * @param	Timeout���ȴ���ʱʱ��
  * @return ACK��ȷ����
  */
static uint8_t FPM_WorkFlow(FPM_HandlerTypeDef *hfpm, uint8_t *pBuff, uint8_t size, uint32_t Timeout)
{
	uint16_t sum[2] = {0};
	
	/* ���Ͳ���ָ�� */
	hfpm->write(pBuff, size);
	hfpm->read(hfpm->pRecvBuff);
	
	/* �ȴ����ջ�ִ,����������ʽ */
	for(; Timeout>0; Timeout--)
	{
		hfpm->delay(1);
		if(*hfpm->pRecvFlag)
		{
			/* �����־λ */
			*hfpm->pRecvFlag = 0;
			break;
		}
	}
	
	/* У�����֤ */
	sum[0] = FPM_Cmd_CheckSum(hfpm->pRecvBuff, *hfpm->pRecvCnt);
	sum[1] = (hfpm->pRecvBuff[*hfpm->pRecvCnt-2] << 8) + hfpm->pRecvBuff[*hfpm->pRecvCnt-1];
	
	return ((sum[0] == sum[1]) ? hfpm->pRecvBuff[9] : FPM_ERR);
}


/**
	* @brief	ָ��ģ��˯�ߺ���
	* @param 	hfpm��ָ��ģ����
	* @return ACK��ȷ����
	*/
uint8_t FPM_Sleep(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_SleepBuffer, sizeof(PS_SleepBuffer), 2000);
}


/**
	* @brief	��ȡͼ��ָ���
	* @param 	hfpm��ָ��ģ����
	* @return ACK��ȷ����
	*/
uint8_t FPM_GetImage(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_GetImageBuffer, sizeof(PS_GetImageBuffer), 2000);
}


/**
	* @brief	����ͼ������ָ���
	* @param 	hfpm��ָ��ģ����
	* @return ACK��ȷ����
	*/
uint8_t FPM_GenChar1(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_GetChar1Buffer, sizeof(PS_GetChar1Buffer), 2000);
}


/**
	* @brief	����ͼ��ģ�庯��
	* @param 	hfpm��ָ��ģ����
	* @return ACK��ȷ����
	*/
uint8_t FPM_SearchMB(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_SearchMBBuffer, sizeof(PS_SearchMBBuffer), 2000);
}


/**
	* @brief	���ָ��ģ�麯��
	* @param 	hfpm��ָ��ģ����
	* @return ACK��ȷ����
	*/
uint8_t FPM_Empty(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_EmptyBuffer, sizeof(PS_EmptyBuffer), 2000);
}


/**
	* @brief	LED�ƿ��ƺ���
	* @param 	hfpm��ָ��ģ����
	* @param 	pColorBuff��ָ�����ɫ�����ָ��
	* @return ACK��ȷ����
	*/
uint8_t FPM_ControlLED(FPM_HandlerTypeDef *hfpm, uint8_t *pColorBuff)
{
	return FPM_WorkFlow(hfpm, pColorBuff, 16, 2000);
}


/**
	* @brief	ɾ��ָ��ID����
	* @param 	hfpm��ָ��ģ����
	* @param 	PageID��ָ��ID��
	* @return ACK��ȷ����
	*/
uint8_t FPM_Delete(FPM_HandlerTypeDef *hfpm, uint16_t PageID)
{
	uint8_t delBuff[16] = {0};
	
	/* �������� */
	memcpy(delBuff, PS_DeleteBuffer, 16);
	/* ����Ҫɾ����ָ��ID��ӵ�ָ���� */
  delBuff[10] = (uint8_t)(PageID>>8);
  delBuff[11] = (uint8_t)PageID;
	/* ���¼���У�� */
  delBuff[14] = FPM_Cmd_CheckSum(delBuff, 16);
  
	return FPM_WorkFlow(hfpm, delBuff, sizeof(delBuff), 2000);
}


/**
	* @brief	�ֲ�ʽ����ָ��ID����
	* @param 	hfpm��ָ��ģ����
	* @return ����ID�Ż������
	*/
uint16_t FPM_Identify(FPM_HandlerTypeDef *hfpm)
{
	uint16_t MatchID;
	if(FPM_GetImage(hfpm) == FPM_OK)
	{
		if(FPM_GenChar1(hfpm) == FPM_OK)
		{
			if(FPM_SearchMB(hfpm) == FPM_OK)
			{
				/* ����ָ��ID */
				MatchID = (hfpm->pRecvBuff[10] << 8) + hfpm->pRecvBuff[11];
				/* ִ���û����� */
				FPM_TrigEvenCallback();
				/* ��˸�����̵� */
				FPM_ControlLED(hfpm, PS_GreenLEDBuffer);
				return	MatchID;
			}
		}
	}
	/* ��һ����ʧ�ܼ�Ϊʧ�� ��˸���κ�� */
	FPM_ControlLED(hfpm, PS_RedLEDBuffer);
	return FPM_NOTFOUND;
}


/**
	* @brief	ע��ָ��ID����
	* @param 	hfpm��ָ��ģ����
	* @param 	PageID��ָ��ID��
	* @param 	Timeout���ȴ���ʱʱ��
	* @return ACK��ȷ����
	*/
uint8_t FPM_Enroll(FPM_HandlerTypeDef *hfpm, uint16_t g_PageID, uint16_t Timeout)
{
	uint8_t EnrollBuff[17] = {0};
	uint16_t sum;
	/* �������� */
	memcpy(EnrollBuff, PS_AutoEnrollBuffer, 17);
	/* ����Ҫɾ����ָ��ID��ӵ�ָ���� */
  EnrollBuff[10] = (uint8_t)(g_PageID>>8);
  EnrollBuff[11] = (uint8_t)g_PageID;
	/* ���¼���У�� */
	sum = FPM_Cmd_CheckSum(EnrollBuff, 17);
  EnrollBuff[15] = (uint8_t)(sum >> 8);
	EnrollBuff[16] = (uint8_t)sum;
	
	/* ���Ƴ��� */
	FPM_ControlLED(hfpm, PS_BlueLEDBuffer);
	
	/* �ж�ע��״̬ */
	if(FPM_WorkFlow(hfpm, EnrollBuff, 17, Timeout) == FPM_OK)
	{
		/* �ɹ���˸�̵� */
		FPM_ControlLED(hfpm, PS_GreenLEDBuffer);
		return FPM_OK;
	}
	else
	{
		/* ʧ�ܺ�ȡ��ָ���˸��� */
		hfpm->write(PS_CancelBuffer,12);
		hfpm->delay(50);
		FPM_ControlLED(hfpm, PS_RedLEDBuffer);
		return FPM_ERR;
	}
}

