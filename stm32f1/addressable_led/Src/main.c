/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
typedef struct{
	uint16_t *pwm_table;
	uint16_t index;
	uint16_t size;
}pwm_list_t;

/*pwm single signal, */
//#define ONE_SEC_TESD
#define ADDRESSABLE_LED

#ifdef ADDRESSABLE_LED
	#define TIMER_PERIOD ((SystemCoreClock / 800000) - 1)
	#define TIMER_PRESCALER 0

	#define W1 52
	#define W0 26
//	uint32_t uhTimerPeriod = (uint32_t)((SystemCoreClock / 800000) - 1); //to get

	#define PWM_PERIOD  W0
	#define BLINK_PERIOD_LED ((uint32_t)10)
	//4 IS ZERO 8 IS 1
	uint16_t pwm_value[] ={

		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W0,W0,W0,W0,

		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W0,W0,W0,W0,W0,W0,W0,

		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W1,W1,W1,W1,W1,

		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W0,W1,W1,W1,W1,



			W0,W0,W0,W0,W1,W1,W0,W1,
			W0,W1,W0,W0,W1,W0,W0,W0,
			W0,W0,W0,W1,W0,W1,W0,W0,

			W0,W0,W0,W1,W0,W0,W0,W0,
			W0,W0,W0,W1,W0,W0,W0,W0,
			W0,W0,W0,W1,W0,W0,W0,W0,

			W0,W0,W0,W1,W0,W0,W0,W0,
			W0,W0,W0,W0,W0,W0,W0,W0,
			W0,W0,W0,W1,W0,W0,W0,W0,

			W0,W0,W0,W0,W1,W1,W1,W1,
			W0,W0,W1,W1,W0,W0,W0,W0,
			W0,W0,W0,W0,W1,W1,W1,W1,

			W0,W0,W0,W1,W0,W0,W0,W0,
			W0,W0,W0,W0,W1,W0,W0,W0,
			W0,W0,W0,W1,W0,W1,W1,W0,

		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,

		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,

		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0
};


uint16_t pwm_value_2[] ={

	W0,W0,W0,W0,W1,W1,W1,W1,
	W0,W0,W0,W0,W0,W0,W0,W0,
	W0,W0,W0,W0,W0,W0,W0,W0,

	W0,W0,W0,W0,W0,W0,W0,W0,
	W0,W0,W0,W0,W1,W1,W1,W1,
	W0,W0,W0,W0,W0,W0,W0,W0,

	W0,W0,W0,W0,W0,W0,W0,W0,
	W0,W0,W0,W0,W0,W0,W0,W0,
	W0,W0,W0,W1,W1,W1,W1,W1,

	W0,W0,W0,W0,W1,W1,W1,W1,
	W0,W0,W0,W0,W0,W0,W0,W0,
	W0,W0,W0,W0,W1,W1,W1,W1,



		W0,W0,W1,W0,W1,W1,W0,W1,
		W0,W1,W0,W0,W1,W0,W0,W0,
		W0,W0,W1,W0,W0,W1,W0,W0,

		W1,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W1,W0,W0,W0,W0,W0,
		W0,W0,W1,W0,W0,W0,W0,W0,

		W0,W0,W0,W1,W0,W0,W0,W0,
		W0,W0,W0,W0,W0,W0,W0,W0,
		W0,W0,W0,W1,W0,W0,W0,W0,

		W0,W0,W0,W0,W1,W1,W1,W1,
		W0,W1,W1,W1,W0,W0,W0,W0,
		W0,W0,W0,W0,W1,W1,W1,W1,

		W0,W1,W0,W1,W0,W0,W0,W0,
		W0,W0,W1,W0,W1,W0,W0,W0,
		W0,W0,W0,W1,W0,W1,W1,W0,

	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,

	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0
};
#endif
#ifdef ONE_SEC_TESD
	#define TIMER_PERIOD 1000
	#define TIMER_PRESCALER 640
	#define PWM_PERIOD  20
	#define BLINK_PERIOD_LED ((uint32_t)10)
	uint16_t pwm_value[69] =
/*	{0000  , 0000 ,  0000,
	 0000  , 0000 ,  0000,
	 4000  , 0000 ,  2000,
	 1000  ,  200 ,  1000,
	 2000  , 0000 ,  4000,
	 5000  , 0000 ,  7000,
	 8000  , 0000 ,  9980};
*/
{
 20  ,  20 ,  20,
 50  ,  50 ,  50,
 100  ,  100 ,  100,
 200  ,  200 ,  200,
 300  , 300 ,  300,
 400  , 400 ,  400,
 500  , 500 ,  500,
 600  , 600 ,  600,
 700  , 700 ,  700,
 800  , 800 ,  800,
 900  , 900 ,  900,

 990  , 990 ,  990,
 900  , 900 ,  900,
 800  , 800 ,  800,
 700  , 700 ,  700,
 600  , 600 ,  600,
 500  , 500 ,  500,
 400  , 400 ,  400,
 300  , 300 ,  300,
 200  , 200 ,  200,
 100  ,  100 ,  100,
 50  ,  50 ,  50,
 20  ,  20 ,  20,
};

#endif

pwm_list_t pwm_list= {pwm_value, 0,(sizeof(pwm_value)/2)-1};

volatile uint32_t blink_period = BLINK_PERIOD_LED;

 void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim)
{
	// if (htim->Instance ==TIM3 ){
	//	pwm_list.pwm_table = 0;
	// }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	//HAL_TIM_PWM_Stop_DMA(&htim3, TIM_CHANNEL_4);
	// if (htim->Instance ==TIM3 ){
		// pwm_list.pwm_table = 0;
	// }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{



	// if (blink_period){
		// blink_period--;
		// return;
	// }
#ifdef USE_TIMER_NOT_DMA
	if (pwm_list.index < pwm_list.size )
	{
		pwm_list.index++;
	}else{
		pwm_list.index = 0;
	}
	TIM3->CCR4 = pwm_list.pwm_table[pwm_list.index];
#endif
	blink_period = BLINK_PERIOD_LED;
	GPIOC->ODR ^=GPIO_PIN_13;
}
// GPIOC->ODR ^=GPIO_PIN_13;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim3_ch4_up;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
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
	setvbuf(stdout,0, _IOLBF, 0);

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	uint32_t seconds = 0;
//	HAL_TIM_Base_Start_IT(&htim3);
	HAL_TIM_Base_Start(&htim3);

	//HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
	printf("Program stars!!\r\n" );
	printf("PWM table size[%d] !!\r\n",sizeof(pwm_value)/2 );
	HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_4, (uint32_t *)pwm_value, sizeof(pwm_value)/2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */


		 if ( HAL_GetTick() >seconds){
			seconds = HAL_GetTick() + 1000;
			printf("LOOPING \r\n");
			/// GPIOC->ODR ^=GPIO_PIN_13;
		 }
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

  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = TIMER_PRESCALER;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = TIMER_PERIOD;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode   = TIM_OCFAST_ENABLE;
  sConfigOC.OCIdleState  = TIM_OCIDLESTATE_SET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.Pulse = PWM_PERIOD;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
