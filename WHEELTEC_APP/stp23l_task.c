#include "bsp_stp23L.h"

//C Lib Include File
#include <stdio.h>

//FreeRTOS Include File
#include "FreeRTOS.h"
#include "queue.h"

static OriData_STP23L_t queue_recv;

void STP23L_Task(void *param)
{
	extern QueueHandle_t g_xQueuestp23L_Ori;
	
	while(1)
	{
		//读取STP23L数据队列
		xQueueReceive(g_xQueuestp23L_Ori,&queue_recv, portMAX_DELAY ); //等待队列数据
		
		//解析STP23L原始数据,得到高度数据
		stp23L_callback(&queue_recv);
		
	}
}


