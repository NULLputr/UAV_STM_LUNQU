#ifndef __BSP_IMU_H
#define __BSP_IMU_H

#include <stdint.h>

typedef struct{
	float x;
	float y;
	float z;
}PrivateBuf_t;

typedef struct{
	PrivateBuf_t gyro;
	PrivateBuf_t accel;
	PrivateBuf_t magn;
}IMU_DATA_t;

typedef struct {
	float roll;
	float pitch;
	float yaw;
}ATTITUDE_DATA_t;

typedef struct{
	uint8_t (*Init)(void);
	uint8_t (*DeInit)(void);
	
	void (*UpdateZeroPoint_axis)(const IMU_DATA_t* point);
	void (*UpdateZeroPoint_attitude)(const ATTITUDE_DATA_t* attitude);
	void (*Update_9axisVal)(IMU_DATA_t* imudata);
	void (*UpdateAttitude)(IMU_DATA_t imudata,ATTITUDE_DATA_t *attitude);
}IMUInterface_t,*pIMUInterface_t;

extern IMUInterface_t UserICM20948;

#endif
