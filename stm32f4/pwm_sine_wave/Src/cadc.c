#include "cadc.h"


#ifdef ADC_DEBUG
	#define DBG(...) printf(__VA_ARGS__);
#else
	#define DBG(...)
#endif
uint32_t adc_read_value;
extern ADC_HandleTypeDef hadc1;

uint8_t half_conversion_cplt_f;
uint8_t conversion_cplt_f;
#define ADC_DMA_BUFF_SIZE (1000)
uint32_t adc_read_buff[ADC_DMA_BUFF_SIZE];

void cadc_poll_reading(void)
{
	if (HAL_ADC_PollForConversion(&hadc1, 200) == HAL_OK)
	{
		 adc_read_value = HAL_ADC_GetValue(&hadc1);
	}
}

uint32_t cadc_get_adc_value(uint8_t channel)
{

	return adc_read_value ;
}





void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
   // adc_read_value = HAL_ADC_GetValue(hadc);
    conversion_cplt_f = 1;
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc)
{
    half_conversion_cplt_f = 1;
}
uint8_t cadc_convertion_done(void){

	return ( conversion_cplt_f | half_conversion_cplt_f);

}




void cadc_setup_adc(void)
{
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start_IT(&hadc1);
	HAL_ADC_Start_DMA(&hadc1, &adc_read_buff[0], ADC_DMA_BUFF_SIZE);
}

void cadc_dma_routine(void){
	#define ADC_DMA_HALF_BUFF_SIZE (ADC_DMA_BUFF_SIZE / 2)

	DBG("\r\n\r\n  [ADC convertion]\r\n");

	if (half_conversion_cplt_f != 0)
	{
		half_conversion_cplt_f = 0;
		DBG(" half dma \r\n");

		uint32_t *adc_read_buff_first_half = &adc_read_buff[0];
		for (int adc_value_idx = 0; adc_value_idx <  ADC_DMA_HALF_BUFF_SIZE; ++adc_value_idx)
		{
			DBG(" %d ", *(adc_read_buff_first_half++));
		}
	}

	if (conversion_cplt_f != 0)
	{
		conversion_cplt_f = 0;
		DBG(" full dma \r\n");
		uint32_t *adc_read_buff_second_half = &adc_read_buff[0];
		for (int adc_value_idx = 0; adc_value_idx <  ADC_DMA_HALF_BUFF_SIZE; ++adc_value_idx)
		{
			DBG(" %x ", *(adc_read_buff_second_half++));
		}
	}
}
