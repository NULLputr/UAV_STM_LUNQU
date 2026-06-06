#ifndef __BALANCE
#define __BALANCE

typedef struct{
	float pitch;
	float roll;
	float yaw;
	float gyrox;
	float gyroy;
	float gyroz;
	float targetPitch;
	float targetRoll;
	float targetYaw;
	float targetHeight;
}DebugShowType_t;


//对外调试监控数据
extern DebugShowType_t debugshow;


#endif

