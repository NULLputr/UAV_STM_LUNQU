#include <stdio.h>
#include <string.h>

//HAL_Include File
#include "usart.h"

//FreeRtos Include File
#include "FreeRTOS.h"
#include "task.h"

//BSP include
#include "bsp_imu.h"
#include "bsp_adc.h"

//APP Include File
#include "balance.h"


//extern ATTITUDE_DATA_t attitude;
////extern uint16_t balance_task_freq;
//extern float get_imu_data_time;

static char showbuffer[256];
static char showTestBuffer[20];

void ShowTask(void *param)
{
	pADCInterface_t adc = &UserADC1;
	
	while(1)
	{
//		float vol = adc->getValue(userconfigADC_VBAT_CHANNEL);
//		
//		vol = (vol/4095.0f)*3.3f * 11.0f;
//		//printf("time = %.3f\r\n",vol);
//		
//		float cur = adc->getValue(userconfigADC_Curr_CHANNEL);
//		
//		cur = ((cur/4095.0f) * 3.3f) / 0.2f;
		
		//printf("cur = %.3f\r\n",cur);
		
		//printf("{B%.3f:%.3f:%.3f}$",attitude.pitch*57.3f,attitude.roll*57.3f,attitude.yaw*57.3f);
		//printf("time = %.3f\r\n",get_imu_data_time);
		//printf("freq = %d\r\n",balance_task_freq);
//		sprintf(showbuffer,"{B%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f}$",
//		debugshow.pitch,
//		debugshow.targetPitch,
//		debugshow.roll,
//		debugshow.targetRoll,
//		debugshow.yaw,
//		debugshow.targetYaw,
//		0.0f,
//		0.0f,
//		0.0f,
//		0.0f
//		); 
		
		//printf("show Task start!\n");
//		printf("{B%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f:%.3f}$",
//		debugshow.pitch,
//		debugshow.targetPitch,
//		debugshow.roll,
//		debugshow.targetRoll,
//		debugshow.yaw,
//		debugshow.targetYaw,
//		0.0f,
//		0.0f,
//		0.0f,
//		0.0f);
	sprintf(showTestBuffer,"buletee test\n");
	
		//通过DMA传输数据
		//HAL_UART_Transmit_DMA(&huart4,(uint8_t*)showbuffer,strlen(showbuffer));
		HAL_UART_Transmit_DMA(&huart4,(uint8_t*)showTestBuffer,strlen(showTestBuffer));
		vTaskDelay(pdMS_TO_TICKS(50));
	}
}
