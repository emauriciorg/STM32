
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2019 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include <stdio.h>

#include "stm32f4xx_hal.h"

#define ADC_BLOCKING_MODE  0
#define ADC_INTERRUPT_MODE 1
#define ADC_DMA_MODE       2

#define ADC_RUNNING_MODE ADC_DMA_MODE

#define HEARTBEAT_PERIOD (1000)

#define ADC_DMA_HALF_BUFF_SIZE (500)
#define ADC_DMA_BUFF_SIZE      (2 * ADC_DMA_HALF_BUFF_SIZE)

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

UART_HandleTypeDef huart1;

static uint32_t measurements_count = 0;

#if (ADC_RUNNING_MODE == ADC_BLOCKING_MODE) || (ADC_RUNNING_MODE == ADC_INTERRUPT_MODE)
static uint32_t adc_read_value;
#endif

#if (ADC_RUNNING_MODE == ADC_INTERRUPT_MODE) || (ADC_RUNNING_MODE == ADC_DMA_MODE)
static uint8_t conversion_cplt_f = 0;
#endif

#if ADC_RUNNING_MODE == ADC_DMA_MODE
static uint8_t half_conversion_cplt_f = 0;

static uint32_t adc_read_buff[ADC_DMA_BUFF_SIZE];

static uint32_t *adc_read_first_half_buff  = &adc_read_buff[0];
static uint32_t *adc_read_second_half_buff = &adc_read_buff[ADC_DMA_HALF_BUFF_SIZE];
#endif

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_USART1_UART_Init(void);

#if (ADC_RUNNING_MODE == ADC_INTERRUPT_MODE) || (ADC_RUNNING_MODE == ADC_DMA_MODE)
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
#	if ADC_RUNNING_MODE == ADC_INTERRUPT_MODE
	adc_read_value = HAL_ADC_GetValue(hadc);
	measurements_count++;
#	elif ADC_RUNNING_MODE == ADC_DMA_MODE
	measurements_count += ADC_DMA_HALF_BUFF_SIZE;
#	endif

	conversion_cplt_f = 1;
}
#endif

#if ADC_RUNNING_MODE == ADC_DMA_MODE
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
	measurements_count += ADC_DMA_HALF_BUFF_SIZE;

	half_conversion_cplt_f = 1;
}
#endif

/**
 * @brief  The application entry point.
 *
 * @retval None
 */
int main(void)
{
	/* Disable buffering for stdout */
	setvbuf(stdout, NULL, _IONBF, 0);

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* Configure the system clock */
	SystemClock_Config();

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART1_UART_Init();

	/* Initial debug message */
	printf("STM32F4 ADC\r\n");

	/* Enable ADC peripheral to start the conversion */
#if ADC_RUNNING_MODE == ADC_BLOCKING_MODE
	HAL_ADC_Start(&hadc1);
#elif ADC_RUNNING_MODE == ADC_INTERRUPT_MODE
	HAL_ADC_Start_IT(&hadc1);
#elif ADC_RUNNING_MODE == ADC_DMA_MODE
	HAL_ADC_Start_DMA(&hadc1, &adc_read_buff[0], ADC_DMA_BUFF_SIZE);
#endif

	/* Initial output status for LED's in DISC0 board */
	/* NOTE: you can move the initial status to MX_GPIO_Init */
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);

	/* Create and initialize program variables */
	uint32_t heartbeat_timer = HAL_GetTick();

#if ADC_RUNNING_MODE == ADC_DMA_MODE
	int adc_value_idx = 0;
#endif

	/* Infinite loop */
	while (1)
	{
		if ((HAL_GetTick() - heartbeat_timer) >= HEARTBEAT_PERIOD)
		{
			heartbeat_timer = HAL_GetTick();

			HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
			HAL_GPIO_TogglePin(LED4_GPIO_Port, LED4_Pin);
		}

#if ADC_RUNNING_MODE == ADC_BLOCKING_MODE
		if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK)
		{
			adc_read_value = HAL_ADC_GetValue(&hadc1);
			measurements_count++;
			printf("ADC measure #%010lu: %04lu\r", measurements_count, adc_read_value);
		}
#elif ADC_RUNNING_MODE == ADC_INTERRUPT_MODE
		if (conversion_cplt_f != 0)
		{
			conversion_cplt_f = 0;
			printf("ADC measure #%010lu: %04lu\r", measurements_count, adc_read_value);
		}
#elif ADC_RUNNING_MODE == ADC_DMA_MODE
		if (half_conversion_cplt_f != 0)
		{
			half_conversion_cplt_f = 0;
			for (adc_value_idx = 0; adc_value_idx < ADC_DMA_HALF_BUFF_SIZE; ++adc_value_idx)
			{
				printf("ADC measures #%010lu: %04lu\r", measurements_count, adc_read_first_half_buff[adc_value_idx]);
			}
		}
		else if (conversion_cplt_f != 0)
		{
			conversion_cplt_f = 0;
			for (adc_value_idx = 0; adc_value_idx < ADC_DMA_HALF_BUFF_SIZE; ++adc_value_idx)
			{
				printf("ADC measures #%010lu: %04lu\r", measurements_count, adc_read_second_half_buff[adc_value_idx]);
			}
		}
#endif
	}
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;

	/**Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 4;
	RCC_OscInitStruct.PLL.PLLN = 180;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Activate the Over-Drive mode
	 */
	if (HAL_PWREx_EnableOverDrive() != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	 */
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick
	 */
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void)
{

	ADC_ChannelConfTypeDef sConfig;

	/**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = DISABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = 1;
#if ADC_RUNNING_MODE == ADC_BLOCKING_MODE
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
#else
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
#endif
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

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
		_Error_Handler(__FILE__, __LINE__);
	}

}

/** 
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) 
{
	/* DMA controller clock enable */
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA2_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/** Configure pins as 
 * Analog
 * Input
 * Output
 * EVENT_OUT
 * EXTI
 */
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, LED3_Pin|LED4_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : LED3_Pin LED4_Pin */
	GPIO_InitStruct.Pin = LED3_Pin|LED4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  file: The file name as string.
 * @param  line: The line in file as a number.
 * @retval None
 */
void _Error_Handler(char *file, int line)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
