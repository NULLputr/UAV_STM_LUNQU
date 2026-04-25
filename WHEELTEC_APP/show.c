#include <stdio.h>

//FreeRtos Include File
#include "FreeRTOS.h"
#include "task.h"

//BSP include
#include "bsp_imu.h"

extern ATTITUDE_DATA_t attitude;
//extern uint16_t balance_task_freq;
extern float get_imu_data_time;

void ShowTask(void *param)
{
	while(1)
	{
		//printf("{B%.3f:%.3f:%.3f}$",attitude.pitch*57.3f,attitude.roll*57.3f,attitude.yaw*57.3f);
		//printf("time = %.3f\r\n",get_imu_data_time);
		//printf("freq = %d\r\n",balance_task_freq);
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}
