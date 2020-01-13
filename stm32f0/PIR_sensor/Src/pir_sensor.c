#include "pir_sensor.h"
#include "stm32f0xx_hal.h"
#include "main.h"


extern TIM_HandleTypeDef htim1;

void pir_init(void){

}



void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	LD4_GPIO_Port->ODR ^=LD4_Pin;
}



void pir_service(void)
{


}
