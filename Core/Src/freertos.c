/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"        
#include "semphr.h"      
#include "event_groups.h" 
#include <stdio.h>
#include "tim.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define userconfig_OPEN_CPU_USAGE_CHECK 0
#define userconfig_OPEN_STACK_CHECK 0
#define userconfig_OPEN_CHECK_HEAPSIZE 0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
xSemaphoreHandle HandleMutex_printf;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
//RTOSϵͳ�������
#if ( 1 == userconfig_OPEN_CPU_USAGE_CHECK ) || ( 1 == userconfig_OPEN_STACK_CHECK ) || ( 1 == userconfig_OPEN_CHECK_HEAPSIZE )
void CpuUsageCheckTask(void *param);
#endif

void StartDefaultTask(void *argument);
void Balance_Task(void *param);
void ShowTask(void *param);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void configureTimerForRunTimeStats(void);
unsigned long getRunTimeCounterValue(void);
void vApplicationIdleHook(void);
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 1 */
/* Functions needed when configGENERATE_RUN_TIME_STATS is on */
__weak void configureTimerForRunTimeStats(void)
{
	//HAL_TIM_Base_Start(&htim6);
		TIM6->CNT = 0;
}

__weak unsigned long getRunTimeCounterValue(void)
{
	#if 1
	static unsigned long time = 0 ;
	static uint16_t lasttime = 0;
	static uint16_t nowtime = 0;
	
	nowtime = TIM6->CNT; //��õ�ǰ����ֵ
	
	//������μ���ֵС���ϴμ���ֵ��˵�������˶�ʱ���������
	if( nowtime < lasttime )
	{
		time += (nowtime + 0xffff - lasttime); //������ʱ������
	}		
	else time += ( nowtime - lasttime ) ; //δ�������������Ϊ����ʱ��-�ϴ�ʱ��
	
	lasttime = nowtime;
	
	return time;
	#endif
	//return 0;
}
/* USER CODE END 1 */

/* USER CODE BEGIN 2 */
void vApplicationIdleHook( void )
{
   /* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
   to 1 in FreeRTOSConfig.h. It will be called on each iteration of the idle
   task. It is essential that code added to this hook function never attempts
   to block in any way (for example, call xQueueReceive() with a block time
   specified, or call vTaskDelay()). If the application makes use of the
   vTaskDelete() API function (as this demo application does) then it is also
   important that vApplicationIdleHook() is permitted to return to its calling
   function, because it is the responsibility of the idle task to clean up
   memory allocated by the kernel to any task that has since been deleted. */
}
/* USER CODE END 2 */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, signed char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
	//printf("%s stack overflow\r\n",pcTaskName);
	  // ???????????(??,????)
  char *str = "Stack Overflow!\r\n";
  while(*str)
  {
    while(!(USART1->SR & UART_FLAG_TXE));
    USART1->DR = (uint8_t)*str++;
  }
  while(1);
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
	//printf("Malloc Faile,check total heap size config\r\n");
	  // ???????????(??,????)
  char *str = "Malloc Faile,check total heap size config\r\n";
  while(*str)
  {
    while(!(USART1->SR & UART_FLAG_TXE));
    USART1->DR = (uint8_t)*str++;
  }
  while(1);
}
/* USER CODE END 5 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	 static uint16_t delaytime = 5000;//打印时间间隔，单位tick
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
		HandleMutex_printf = xSemaphoreCreateMutex();

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	
	//balance task
	xTaskCreate(Balance_Task,"balance",128*4,NULL,osPriorityNormal,NULL);
	
	xTaskCreate(ShowTask,"showtask",128*4,NULL,osPriorityBelowNormal7,NULL);
	
	//调试任务
	#if ( 1 == userconfig_OPEN_CPU_USAGE_CHECK ) || ( 1 == userconfig_OPEN_STACK_CHECK ) || ( 1 == userconfig_OPEN_CHECK_HEAPSIZE )

	 xTaskCreate(CpuUsageCheckTask,"DebugTask",512, &delaytime ,osPriorityAboveNormal,NULL);
	#endif
	
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
		HAL_GPIO_TogglePin(UserLED1_GPIO_Port,UserLED1_Pin);
		HAL_GPIO_TogglePin(UserLED2_GPIO_Port,UserLED2_Pin);
		#if 0
		char *str = "LED On Or LED Off!\r\n";
		while(*str)
		{
			while(!(USART1->SR & UART_FLAG_TXE));
			USART1->DR = (uint8_t)*str++;
		}
		#endif
		//while(1);
		vTaskDelay(pdMS_TO_TICKS(500));
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
#if ( 1 == userconfig_OPEN_CPU_USAGE_CHECK ) || ( 1 == userconfig_OPEN_STACK_CHECK ) || ( 1 == userconfig_OPEN_CHECK_HEAPSIZE )
void CpuUsageCheckTask(void *param)
{
	uint16_t* delaytime = (uint16_t*)param;
	static char showbuf[500];
	
	while( 1 )
	{
#if 1 == userconfig_OPEN_CPU_USAGE_CHECK
		vTaskGetRunTimeStats(showbuf);
		xSemaphoreTake(HandleMutex_printf,portMAX_DELAY);
		printf("TaskName\tUseTime\tCPU\r\n");
		printf("%s\r\n",showbuf);
		xSemaphoreGive(HandleMutex_printf); // ????!
#endif

#if 1 == userconfig_OPEN_STACK_CHECK
		vTaskList(showbuf);
		xSemaphoreTake(HandleMutex_printf,portMAX_DELAY);
		printf("TaskName\tTaskState\tTaskPrio\tStackSize\tTaskNum\r\n");
		printf("%s\r\n",showbuf);
		xSemaphoreGive(HandleMutex_printf);
#endif
		
#if 1 == userconfig_OPEN_CHECK_HEAPSIZE
		xSemaphoreTake(HandleMutex_printf,portMAX_DELAY);
		printf("free heap size : %d bytes\r\n\r\n",xPortGetFreeHeapSize());
		xSemaphoreGive(HandleMutex_printf);
#endif
		// ?????????!!!
		vTaskDelay(*delaytime);
	}
}
#endif
/* USER CODE END Application */

