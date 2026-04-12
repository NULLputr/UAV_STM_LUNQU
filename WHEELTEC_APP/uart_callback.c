#include "usart.h"

//FreeRTOS Include File
#include "FreeRTOS.h"

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
