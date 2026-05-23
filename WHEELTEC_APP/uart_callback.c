#include "usart.h"

//C Lib Include File
#include <stdio.h>


//FreeRTOS Include File
#include "FreeRTOS.h"
//#include "tim.h"
//#include "usart.h"
#include "queue.h"

//BSP Include File
#include "bsp_stp23L.h"

extern uint8_t uart1_recv;
extern uint8_t uart4_recv;

//?????? booter
static void _System_Reset_(uint8_t uart_recv)
{
	extern void ResetSystem(uint8_t isFromISR);
	
	static uint8_t res_buf[5];
	static uint8_t res_count=0;
	
	res_buf[res_count]=uart_recv;
	
	if( uart_recv=='r'||res_count>0 )
		res_count++;
	else
		res_count = 0;
	
	if(res_count==5)
	{
		res_count = 0;
		//接受到上位机请求的复位字符“reset”，执行软件复位
		if( res_buf[0]=='r'&&res_buf[1]=='e'&&res_buf[2]=='s'&&res_buf[3]=='e'&&res_buf[4]=='t' )
		{
			//ResetSystem(1);//进行软件复位，复位后执行 BootLoader 程序
			NVIC_SystemReset();
		}
	}
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	//jiancha shifou you genggaoji de youxianji beihuanxing 
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	
	if( huart == &huart4 ) //蓝牙使用的串口
	{
		//复位BootLoadr检测指令
		_System_Reset_(uart4_recv);
		
		/* APP写入指令 */
//		extern QueueHandle_t g_xQueueBlueTooth_Ori;
//		xQueueSendFromISR(g_xQueueBlueTooth_Ori,&uart4_recv,&xHigherPriorityTaskWoken);
		
		HAL_UART_Receive_IT(&huart4,&uart4_recv,1);
	}
	else if( huart == &huart1 )
	{
		_System_Reset_(uart1_recv);
		HAL_UART_Receive_IT(&huart1,&uart1_recv,1);
	}
	
	//根据具体情况发起调度
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}


//DMA′?ê?íê3éó?°?íê3é?D??
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	#if 1
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if( huart == &huart5 && HAL_UART_STATE_READY == huart->RxState) 
	{	
		extern QueueHandle_t g_xQueuestp23L_Ori;

		
		if(g_xQueuestp23L_Ori != NULL)
		{
			DMABuf_oridata_stp23L.DataLen = Size;
			xQueueSendFromISR(g_xQueuestp23L_Ori,&DMABuf_oridata_stp23L,&xHigherPriorityTaskWoken);
		}
		
				//启动DMA
		HAL_UARTEx_ReceiveToIdle_DMA(&huart5,DMABuf_oridata_stp23L.Buf,userconfig_STP23L_DMABUF_LEN);
	}
	
	//主动发起调度
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	#endif
	
	#if 0
		//检查高优先级任务唤醒
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if( huart == &huart5 && HAL_UART_STATE_READY == huart->RxState ) 
	{	
		//Size存放的内容是DMA已经搬运的数据数量
		
		extern QueueHandle_t g_xQueuestp23L_Ori;
		if( g_xQueuestp23L_Ori!=NULL )
		{
			DMABuf_oridata_stp23L.DataLen = Size;
			xQueueSendFromISR(g_xQueuestp23L_Ori,&DMABuf_oridata_stp23L,&xHigherPriorityTaskWoken);
		}
		
		//直接在中断里解析处理数据，有可能比较耗时，影响其他任务调度的实时性

		//重新启动DMA搬运
		HAL_UARTEx_ReceiveToIdle_DMA(&huart5,DMABuf_oridata_stp23L.Buf,userconfig_STP23L_DMABUF_LEN);
	}
	
	//主动发起调度
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	
	#endif
	
	
}
