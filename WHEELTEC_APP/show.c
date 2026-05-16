#include <stdio.h>

//FreeRtos Include File
#include "FreeRTOS.h"
#include "task.h"

//BSP include
#include "bsp_imu.h"
#include "bsp_adc.h"


extern ATTITUDE_DATA_t attitude;
//extern uint16_t balance_task_freq;
extern float get_imu_data_time;

void ShowTask(void *param)
{
	pADCInterface_t adc = &UserADC1;
	
	while(1)
	{
		float vol = adc->getValue(userconfigADC_Curr_CHANNEL);
		
		vol = (vol/4095.0f)*3.3f * 11.0f;
		printf("time = %.3f\r\n",vol);
		//printf("{B%.3f:%.3f:%.3f}$",attitude.pitch*57.3f,attitude.roll*57.3f,attitude.yaw*57.3f);
		//printf("time = %.3f\r\n",get_imu_data_time);
		//printf("freq = %d\r\n",balance_task_freq);
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}
