//FreeRtos Include File
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

//BSP include
#include "bsp_imu.h"
#include "bsp_RTOSdebug.h"
#include "bsp_dshot.h"

ATTITUDE_DATA_t attitude = {0};

uint16_t balance_task_freq = 0;

float get_imu_data_time = 0.0;

void Balance_Task(void *param)
{
	//get time base
	TickType_t preTime = xTaskGetTickCount();
	
	
	//HZ
	const uint16_t TaskFreq = 100;
	
	pIMUInterface_t imu = &UserICM20948;
	IMU_DATA_t imu_ori = {0};
	pRtosDebugInterface_t debug = &RTOSTaskDebug;
	
	RtosDebugPrivateVar priv = {0};
	
	//ATTITUDE_DATA_t attitude = {0};
	
	pMotorInterface_t dshot = &UserDshotMotor;
	
	
	//定义4个电机结构体
	dshotMotorVal_t m1 = {0,48},m2={0,48},m3={0,48},m4={0,48};
	
	static uint32_t unlock_motor=0;
	
	while(1)
	{
		//balance_task_freq = debug->UpdateFreq(&priv);
		
		debug->TickStart(&priv);
		imu->Update_9axisVal(&imu_ori);
		//printf("freq:%d\r\n",balance_task_freq);
		
		imu->UpdateAttitude(imu_ori,&attitude);
		//printf("{B%.3f:%.3f:%.3f}$",attitude.pitch*57.3f,attitude.roll*57.3f,attitude.yaw*57.3f);
		
		//控制电机测试
		if(unlock_motor < TaskFreq * 4)  //前4秒发送最低油门，解锁电调
		{
			unlock_motor++;

		}
		else
		{
			m1.throttle = 120;
//			m2.throttle = 220;
//			m3.throttle = 120;
//			m4.throttle = 220;			
		}
		
		//发送油门指令到电调,先发送最低油门一段时间进行解锁
		dshot->set_target(m1,m2,m3,m4);	
		//get_imu_data_time = debug->UpdateUsedTime(&priv);
		vTaskDelayUntil(&preTime,pdMS_TO_TICKS((1.0f/(float)TaskFreq) * 1000));
		
	}
	
}

