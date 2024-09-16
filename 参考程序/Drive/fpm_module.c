/* Description ---------------------------------------------------------------*/

/* Includes ------------------------------------------------------------------*/
#include "fpm_module.h"
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* 睡眠指令 */
uint8_t PS_SleepBuffer[12] 			= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x33,0x00,0x37};
/* 清空指纹指令 */
uint8_t PS_EmptyBuffer[12] 			= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x0D,0x00,0x11};
/* 取消指令 */
uint8_t PS_CancelBuffer[12] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x30,0x00,0x34};
/* 蓝色LED灯开启指令 */
uint8_t PS_BlueLEDBuffer[16] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x03,0x01,0x01,0x00,0x00,0x49};
/* 红色LED灯双闪指令 */
uint8_t PS_RedLEDBuffer[16] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x04,0x04,0x02,0x00,0x50};
/* 绿色LED灯双闪指令 */
uint8_t PS_GreenLEDBuffer[16] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x3C,0x02,0x02,0x02,0x02,0x00,0x4C};
/* 自动注册指纹指令 */
uint8_t PS_AutoEnrollBuffer[17] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x31,'\0','\0',0x04,0x00,0x0F,'\0','\0'};
/* 获取图像指令 */
uint8_t PS_GetImageBuffer[12] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};
/* 生成特征1指令 */
uint8_t PS_GetChar1Buffer[13] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};
/* 生成特征2指令 */
uint8_t PS_GetChar2Buffer[13] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};
/* 搜索模板指令 */
uint8_t PS_SearchMBBuffer[17] 	= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0xFF,0xFF,0x02,0x0C};
/* 删除模板指令 */
uint8_t PS_DeleteBuffer[16] 		= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x07,0x0C,'\0','\0',0x00,0x01,'\0','\0'};
/* 获取指纹唯一SN码指令 */
uint8_t PS_GetChipSN[13] 				= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x34,0x00,0x00,0x39};
/* 握手指令 */
uint8_t PS_HandShake[12] 				= {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x35,0x00,0x39};
/* 读取有效模板数量指令 */
uint8_t PS_ValidTemplateNum[12] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1D,0x00,0x21};
/* Global variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/**
  * @brief	指纹验证成功后用户逻辑代码实现，该函数弱定义，建议外部重写
  * @param 	None
  * @return None
  */
__weak void FPM_TrigEvenCallback(void)
{
	/* UNUSED */
}


/**
  * @brief	指纹模块初始化函数，该函数弱定义，建议外部重写
  * @param 	hfpm：指纹模块句柄
  * @return none
  */
__weak void FPM_Init(FPM_HandlerTypeDef *hfpm)
{
	/* USER CODE FOR INITIALIZATION TO STRUCTURE -------------------------------*/
	
	/* Please improve the following code ****
		
		* 延时函数
		hfpm->delay = ****;
		
		* 发送接收函数
		hfpm->write = ****;
		hfpm->read 	= ****;
		
		* 接收状态标志位、大小及缓冲数组
		hfpm->pRecvCnt	= &****;
		hfpm->pRecvFlag = &****;
		hfpm->pRecvBuff = ****;
	
		* 上电初始化后开启睡眠
		FPM_Sleep(hfpm);
	
	*/
}


/**
  * @brief	内部指令校验和函数
  * @param 	pBuff：需要校验的数据
  * @param	size：数据大小
  * @return sum：校验和
  */
static uint16_t FPM_Cmd_CheckSum(uint8_t *pBuff, uint8_t size)
{
	uint16_t sum = 0;
	uint8_t Size = size - 8;
	uint8_t *pData = pBuff + 6;
	
	/* 计算校验和 */
	for(uint8_t i=0; i<Size; i++)
	{
		sum += *(pData+i);
	}
	return sum;
}


/**
  * @brief  指纹模块各指令的收发流程
  * @param	hfpm：指纹模块句柄
  * @param 	pBuff：指向需发送指令的指针
  * @param	size：数据大小
  * @param	Timeout：等待超时时间
  * @return ACK：确认码
  */
static uint8_t FPM_WorkFlow(FPM_HandlerTypeDef *hfpm, uint8_t *pBuff, uint8_t size, uint32_t Timeout)
{
	uint16_t sum[2] = {0};
	
	/* 发送操作指令 */
	hfpm->write(pBuff, size);
	hfpm->read(hfpm->pRecvBuff);
	
	/* 等待接收回执,采用阻塞方式 */
	for(; Timeout>0; Timeout--)
	{
		hfpm->delay(1);
		if(*hfpm->pRecvFlag)
		{
			/* 清除标志位 */
			*hfpm->pRecvFlag = 0;
			break;
		}
	}
	
	/* 校验和验证 */
	sum[0] = FPM_Cmd_CheckSum(hfpm->pRecvBuff, *hfpm->pRecvCnt);
	sum[1] = (hfpm->pRecvBuff[*hfpm->pRecvCnt-2] << 8) + hfpm->pRecvBuff[*hfpm->pRecvCnt-1];
	
	return ((sum[0] == sum[1]) ? hfpm->pRecvBuff[9] : FPM_ERR);
}


/**
	* @brief	指纹模块睡眠函数
	* @param 	hfpm：指纹模块句柄
	* @return ACK：确认码
	*/
uint8_t FPM_Sleep(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_SleepBuffer, sizeof(PS_SleepBuffer), 2000);
}


/**
	* @brief	获取图像指令函数
	* @param 	hfpm：指纹模块句柄
	* @return ACK：确认码
	*/
uint8_t FPM_GetImage(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_GetImageBuffer, sizeof(PS_GetImageBuffer), 2000);
}


/**
	* @brief	生成图像特征指令函数
	* @param 	hfpm：指纹模块句柄
	* @return ACK：确认码
	*/
uint8_t FPM_GenChar1(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_GetChar1Buffer, sizeof(PS_GetChar1Buffer), 2000);
}


/**
	* @brief	搜索图像模板函数
	* @param 	hfpm：指纹模块句柄
	* @return ACK：确认码
	*/
uint8_t FPM_SearchMB(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_SearchMBBuffer, sizeof(PS_SearchMBBuffer), 2000);
}


/**
	* @brief	清空指纹模块函数
	* @param 	hfpm：指纹模块句柄
	* @return ACK：确认码
	*/
uint8_t FPM_Empty(FPM_HandlerTypeDef *hfpm)
{
	return FPM_WorkFlow(hfpm, PS_EmptyBuffer, sizeof(PS_EmptyBuffer), 2000);
}


/**
	* @brief	LED灯控制函数
	* @param 	hfpm：指纹模块句柄
	* @param 	pColorBuff：指向灯颜色数组的指针
	* @return ACK：确认码
	*/
uint8_t FPM_ControlLED(FPM_HandlerTypeDef *hfpm, uint8_t *pColorBuff)
{
	return FPM_WorkFlow(hfpm, pColorBuff, 16, 2000);
}


/**
	* @brief	删除指纹ID函数
	* @param 	hfpm：指纹模块句柄
	* @param 	PageID：指纹ID号
	* @return ACK：确认码
	*/
uint8_t FPM_Delete(FPM_HandlerTypeDef *hfpm, uint16_t PageID)
{
	uint8_t delBuff[16] = {0};
	
	/* 拷贝数组 */
	memcpy(delBuff, PS_DeleteBuffer, 16);
	/* 将需要删除的指纹ID添加到指令中 */
  delBuff[10] = (uint8_t)(PageID>>8);
  delBuff[11] = (uint8_t)PageID;
	/* 重新计算校验 */
  delBuff[14] = FPM_Cmd_CheckSum(delBuff, 16);
  
	return FPM_WorkFlow(hfpm, delBuff, sizeof(delBuff), 2000);
}


/**
	* @brief	分布式搜索指纹ID函数
	* @param 	hfpm：指纹模块句柄
	* @return 返回ID号或错误码
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
				/* 计算指纹ID */
				MatchID = (hfpm->pRecvBuff[10] << 8) + hfpm->pRecvBuff[11];
				/* 执行用户程序 */
				FPM_TrigEvenCallback();
				/* 闪烁两次绿灯 */
				FPM_ControlLED(hfpm, PS_GreenLEDBuffer);
				return	MatchID;
			}
		}
	}
	/* 任一环节失败即为失败 闪烁两次红灯 */
	FPM_ControlLED(hfpm, PS_RedLEDBuffer);
	return FPM_NOTFOUND;
}


/**
	* @brief	注册指纹ID函数
	* @param 	hfpm：指纹模块句柄
	* @param 	PageID：指纹ID号
	* @param 	Timeout：等待超时时间
	* @return ACK：确认码
	*/
uint8_t FPM_Enroll(FPM_HandlerTypeDef *hfpm, uint16_t g_PageID, uint16_t Timeout)
{
	uint8_t EnrollBuff[17] = {0};
	uint16_t sum;
	/* 拷贝数组 */
	memcpy(EnrollBuff, PS_AutoEnrollBuffer, 17);
	/* 将需要删除的指纹ID添加到指令中 */
  EnrollBuff[10] = (uint8_t)(g_PageID>>8);
  EnrollBuff[11] = (uint8_t)g_PageID;
	/* 重新计算校验 */
	sum = FPM_Cmd_CheckSum(EnrollBuff, 17);
  EnrollBuff[15] = (uint8_t)(sum >> 8);
	EnrollBuff[16] = (uint8_t)sum;
	
	/* 蓝灯常亮 */
	FPM_ControlLED(hfpm, PS_BlueLEDBuffer);
	
	/* 判断注册状态 */
	if(FPM_WorkFlow(hfpm, EnrollBuff, 17, Timeout) == FPM_OK)
	{
		/* 成功闪烁绿灯 */
		FPM_ControlLED(hfpm, PS_GreenLEDBuffer);
		return FPM_OK;
	}
	else
	{
		/* 失败后取消指令并闪烁红灯 */
		hfpm->write(PS_CancelBuffer,12);
		hfpm->delay(50);
		FPM_ControlLED(hfpm, PS_RedLEDBuffer);
		return FPM_ERR;
	}
}

