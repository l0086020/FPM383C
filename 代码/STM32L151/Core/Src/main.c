/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FPM383C.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t uart_temp[1];

extern uint8_t ScanStatus;
extern uint8_t USART2_STA;
extern uint8_t USART3_STA;

extern uint16_t PageID,MatchScore;

extern uint8_t USART2_ReceiveBuffer[20];
extern uint8_t USART3_ReceiveBuffer[6];

extern uint8_t PS_BlueLEDBuffer[16];
extern uint8_t PS_RedLEDBuffer[16];
extern uint8_t PS_GreenLEDBuffer[16];

extern const char StringEnroll[7];
extern const char StringEmpty[7];
extern const char StringDelete[7];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_UART_Receive_IT(&huart2,USART2_ReceiveBuffer,sizeof(USART2_ReceiveBuffer));

	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_RXNE);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
	HAL_Delay(100);
	
	FPM383C_Sleep();

	while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5));
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
	
	__HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_5);
	
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
  {
		switch(USART3_STA & 0xF0)
		{
			case 0xC0:
				printf("\r\n——————注册指纹模式——————\r\n");
				printf("\r\n　      >>>请先输入指纹ID<<<　   \r\n");
				USART3_STA = 0;
			
				while(!(USART3_STA & 0x80));

				PageID = atoi((char *)USART3_ReceiveBuffer);
				printf("\r\n　      >>>输入指纹ID:%d<<<　   \r\n",PageID);
				
				ScanStatus |= 1<<3;
				printf("\r\n >>>请将需要注册的手指按压模块4次<<< \r\n");
				USART3_STA = 0;
				break;
				
			case 0xA0:
				printf("\r\n————删除指纹模式————\r\n");
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
				HAL_Delay(200);
			
				USART3_STA = 0;
				memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
				
				printf("\r\n>>>请先输入指纹ID<<<\r\n");
				
				HAL_Delay(5000);
				
				PageID = atoi((char *)USART3_ReceiveBuffer);
				printf("\r\n>>>输入的指纹ID：%d<<<\r\n",PageID);
				
				if(FPM383C_Delete(PageID,2000) == 0x00)
				{
					FPM383C_ControlLED(PS_GreenLEDBuffer,2000);
					printf("\r\n>>>已删除指纹<<<\r\n");
				}
				
				USART3_STA = 0;
				memset(USART3_ReceiveBuffer,0,sizeof(USART3_ReceiveBuffer));
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
				USART3_STA = 0;
				break;
			
			case 0x90:
				printf("\r\n————清空指纹模式————\r\n");
				
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
				HAL_Delay(200);
				
				if(FPM383C_Empty(2000) == 0x00)
				{
					FPM383C_ControlLED(PS_GreenLEDBuffer,1000);
					printf("\r\n　　　>>>已清空指纹<<<　　　\r\n");
				}

				USART3_STA = 0;
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
				break;
		}
		
		switch(ScanStatus)
		{
			case 0x80:
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
				HAL_Delay(100);
				FPM383C_ControlLED(PS_BlueLEDBuffer,1000);
				HAL_Delay(5);
				FPM383C_Identify();						//验证指纹模式
				HAL_Delay(500);
				FPM383C_Sleep();
				while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5));
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
				ScanStatus = 0;
			break;
			
			case 0x88:
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
				HAL_Delay(100);
				FPM383C_ControlLED(PS_BlueLEDBuffer,1000);
				HAL_Delay(5);
				FPM383C_Enroll(PageID,10000);		//注册指纹模式
				HAL_Delay(500);
				FPM383C_Sleep();
				while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5));
				HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
				ScanStatus = 0;
			break;
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
