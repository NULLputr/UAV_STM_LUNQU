#include "balance.h"

//FreeRtos Include File
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"

//BSP include
#include "bsp_imu.h"
#include "bsp_RTOSdebug.h"
#include "bsp_dshot.h"
#include "pid.h"
#include "bsp_adc.h"
#include "bsp_gamepad.h"
#include "WiredlessPS2_gamepad.h"

extern float g_robotVOL;

//PID控制器
extern PIDControllerType_t RollRatePID,RollPID;
extern PIDControllerType_t PitchRatePID,PitchPID;
extern PIDControllerType_t YawRatePID,YawPID;
extern PIDControllerType_t HeightSpeedPID,HeightPID;

extern void PID_Update(PIDControllerType_t* pid,float target,float current);
extern void PID_Reset(PIDControllerType_t* pid);

static 	pIMUInterface_t imu = &UserICM20948;
static ATTITUDE_DATA_t attitude = {0};  //角度值
static IMU_DATA_t imu_ori = {0};				//角速度值

//电压获取相关变量
static pADCInterface_t adc = &UserADC1;
float robot_vol;


//电机的油门值
static pMotorInterface_t dshot = &UserDshotMotor;
static dshotMotorVal_t mA = {0,48},mB={0,48},mC={0,48},mD={0,48};
//freertors 调试接口相关变量
//static pRtosDebugInterface_t debug = &RTOSTaskDebug;
//static RtosDebugPrivateVar priv = {0};
//static uint16_t balance_task_freq = 0;
//static float get_imu_data_time = 0.0;


//用于观察飞行器的状态调试变量
DebugShowType_t debugshow;

//飞行器启动标志
static int StartFlag = 1;


//void WiredlessPS2_KeyEvent_Callback(uint8_t keyid,GamePadKeyEventType_t event)
//{
//	if(keyid == PS2KEY_1GREEN)
//	{
//		mA.throttle += 50;
//		mB.throttle += 50;
//		mC.throttle += 50;
//		mD.throttle += 50;
//		
//		if(mA.throttle >= 2047) mA.throttle = 2047;
//		if(mB.throttle >= 2047) mB.throttle = 2047;
//		if(mC.throttle >= 2047) mC.throttle = 2047;
//		if(mD.throttle >= 2047) mD.throttle = 2047;
//	}
//	else if(keyid == PS2KEY_3BLUE)
//	{
//		mA.throttle -= 50;
//		mB.throttle -= 50;
//		mC.throttle -= 50;
//		mD.throttle -= 50;	

//		if(mA.throttle <= 48) mA.throttle = 48;
//		if(mB.throttle <= 48) mB.throttle = 48;
//		if(mC.throttle <= 48) mC.throttle = 48;
//		if(mD.throttle <= 48) mD.throttle = 48;
//	}
//	else if(keyid == PS2KEY_2RED)
//	{
//		mA.throttle = 48;
//		mB.throttle = 48;
//		mC.throttle = 48;
//		mD.throttle = 48;
//	}
//}

//重量转为油门值,入口参数单位克g
static uint16_t weight_to_throttle(float weight,float height)
{
//	weight = weight + height*14.0f;
	
    return 1537.96f + 8.70f*weight - 225.2f*g_robotVOL - 0.0088f*weight*weight - 0.24f*weight*g_robotVOL + 9.01f*g_robotVOL*g_robotVOL;
}

void Balance_Task(void *param)
{
	//get time base
	TickType_t preTime = xTaskGetTickCount();
	
	
	//HZ
		const uint16_t TaskFreq = 200;
	
//	pIMUInterface_t imu = &UserICM20948;
//	IMU_DATA_t imu_ori = {0};
//	pRtosDebugInterface_t debug = &RTOSTaskDebug;
//	RtosDebugPrivateVar priv = {0};
	
	//ATTITUDE_DATA_t attitude = {0};
	
//	pMotorInterface_t dshot = &UserDshotMotor;
	
	
	//定义4个电机结构体
//	dshotMotorVal_t m1 = {0,48},m2={0,48},m3={0,48},m4={0,48};
	//775
	//剩余油门值 = 1500 - 775 = 725
	//printf("%d\r\n", weight_to_throttle(140.0f,10.0f));
	while(1)
	{
		//balance_task_freq = debug->UpdateFreq(&priv);
		
		//debug->TickStart(&priv);
		imu->Update_9axisVal(&imu_ori);
		//printf("freq:%d\r\n",balance_task_freq);
		
		imu->UpdateAttitude(imu_ori,&attitude);
		
		
		
		robot_vol = adc->getValue(userconfigADC_VBAT_CHANNEL);
		robot_vol = (robot_vol / 4095.0f) * 3.3f * 11.0f;
		

		//printf("{B%.3f:%.3f:%.3f}$",attitude.pitch*57.3f,attitude.roll*57.3f,attitude.yaw*57.3f);
		
		//控制电机测试
//		if(unlock_motor < TaskFreq * 4)  //前4秒发送最低油门，解锁电调
//		{
//			unlock_motor++;

//		}
//		else
//		{
//			m1.throttle = 120;
//			m2.throttle = 220;
//			m3.throttle = 120;
//			m4.throttle = 220;			
//		}

		if(StartFlag == 1)
		{
			//roll-外环-角度
			PID_Update(&RollPID,0,attitude.roll);
			
			//pitch-外环-角度
			PID_Update(&PitchPID,0,attitude.pitch);
			
			//yaw-外环-角度
			PID_Update(&YawPID,0,attitude.yaw);
			
			//rollrate-内环-角速度
			PID_Update(&RollRatePID,RollPID.output,imu_ori.gyro.x);
			
			//pitch-内环-角速度
			PID_Update(&PitchRatePID,PitchPID.output,imu_ori.gyro.y);
			
			//yaw-内环-角速度			
			PID_Update(&YawRatePID,YawPID.output,imu_ori.gyro.z);
			
			float basethro = weight_to_throttle(140.0f,robot_vol);
			
			//当前电机性能：
			//最大油门：1500附近
			//最小油门：48
			//大约700油门值
			//其中yawratePID分配150油门值
			//RollRatePID.output和PitchRatePID.output共同分配550
			
			mA.throttle = basethro - RollRatePID.output - PitchRatePID.output + YawRatePID.output;
			mB.throttle = basethro + RollRatePID.output - PitchRatePID.output - YawRatePID.output;
			mC.throttle = basethro + RollRatePID.output + PitchRatePID.output + YawRatePID.output;
			mD.throttle = basethro - RollRatePID.output + PitchRatePID.output - YawRatePID.output;			
		}
		else
		{
			PID_Reset(&RollPID);
			PID_Reset(&PitchPID);
			PID_Reset(&YawPID);
			PID_Reset(&RollRatePID);
			PID_Reset(&PitchRatePID);
			PID_Reset(&YawRatePID);			
		}


		
		//发送油门指令到电调,先发送最低油门一段时间进行解锁
		dshot->set_target(mA,mD,mC,mB);	
		//get_imu_data_time = debug->UpdateUsedTime(&priv);
		debugshow.gyrox = imu_ori.gyro.x;
		debugshow.gyroy = imu_ori.gyro.y;
		debugshow.gyroz = imu_ori.gyro.z;
		debugshow.pitch = attitude.pitch;
		debugshow.roll = attitude.roll;
		debugshow.yaw = attitude.yaw;
//		debugshow.targetPitch = TargetCmd.pitch
//		debugshow.targetRoll = TargetCmd.roll;
//		debugshow.targetYaw = YawControl;
//		debugshow.targetHeight = 0;
		vTaskDelayUntil(&preTime,pdMS_TO_TICKS((1.0f/(float)TaskFreq) * 1000));
		
	}
	
}

