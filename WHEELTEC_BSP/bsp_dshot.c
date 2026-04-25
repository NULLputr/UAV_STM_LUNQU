#include "bsp_dshot.h"
#include "tim.h"

#define ESC_BIT_1 420
#define ESC_BIT_0 210
#define ESC_CMD_BUF_LEN 20

static uint16_t g_esc_cmd[4][ESC_CMD_BUF_LEN] = {0};

#define NOT_DMA_MOTOR 0

#if 1 == NOT_DMA_MOTOR
static uint16_t g_esc_index = ESC_CMD_BUF_LEN;
#endif


TIM_HandleTypeDef* DshotTIM = &htim8;

static uint16_t DshotDecode(dshotMotorVal_t val)
{
	uint16_t  packet = (val.throttle << 1)|(val.Telemetry ? 1:0);
	
	uint8_t crc = (packet ^ (packet >> 4)^(packet >> 8)) & 0x0F;
	
	packet = (packet << 4) | crc;
	
	return packet;
}

static void pwmWriteDigital(dshotMotorVal_t m1,dshotMotorVal_t m2,dshotMotorVal_t m3,dshotMotorVal_t m4)
{
	m1.throttle = ((m1.throttle > 2047) ? 2047 : m1.throttle);
	m2.throttle = ((m2.throttle > 2047) ? 2047 : m2.throttle);
	m3.throttle = ((m3.throttle > 2047) ? 2047 : m3.throttle);
	m4.throttle = ((m4.throttle > 2047) ? 2047 : m4.throttle);	
	
	m1.throttle = DshotDecode(m1);
	m2.throttle = DshotDecode(m2);
	m3.throttle = DshotDecode(m3);
	m4.throttle = DshotDecode(m4);
	
	uint8_t i = 0;
	
	for(i = 0;i < 16;i++)
	{
		g_esc_cmd[0][i] = ((m1.throttle>> (15 - i)) & 0x01) ? ESC_BIT_1: ESC_BIT_0;
		g_esc_cmd[1][i] = ((m2.throttle>> (15 - i)) & 0x01) ? ESC_BIT_1: ESC_BIT_0;
		g_esc_cmd[2][i] = ((m3.throttle>> (15 - i)) & 0x01) ? ESC_BIT_1: ESC_BIT_0;
		g_esc_cmd[3][i] = ((m4.throttle>> (15 - i)) & 0x01) ? ESC_BIT_1: ESC_BIT_0;
	}
	
	#if NOT_DMA_MOTOR
	while(1) if(ESC_CMD_BUF_LEN == g_esc_index) break;
	g_esc_index = 0;
	HAL_TIM_Base_Start_IT(DshotTIM);
	#else
	HAL_TIM_PWM_Start_DMA(DshotTIM,TIM_CHANNEL_1,(uint32_t*)&g_esc_cmd[0][0],ESC_CMD_BUF_LEN);
	HAL_TIM_PWM_Start_DMA(DshotTIM,TIM_CHANNEL_2,(uint32_t*)&g_esc_cmd[1][0],ESC_CMD_BUF_LEN);
	HAL_TIM_PWM_Start_DMA(DshotTIM,TIM_CHANNEL_3,(uint32_t*)&g_esc_cmd[2][0],ESC_CMD_BUF_LEN);
	HAL_TIM_PWM_Start_DMA(DshotTIM,TIM_CHANNEL_4,(uint32_t*)&g_esc_cmd[3][0],ESC_CMD_BUF_LEN);
	#endif
	
}

#if 1 == NOT_DMA_MOTOR
void User_TIM8_UpdateCallback(void)
{
	__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_1,g_esc_cmd[0][g_esc_index]);
	__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_2,g_esc_cmd[1][g_esc_index]);
	__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_3,g_esc_cmd[2][g_esc_index]);
	__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_4,g_esc_cmd[3][g_esc_index]);

	g_esc_index++;

	if(g_esc_index == ESC_CMD_BUF_LEN)
	{
		HAL_TIM_Base_Stop_IT(DshotTIM);
		__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_1,0);
		__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_2,0);
		__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_3,0);
		__HAL_TIM_SET_COMPARE(DshotTIM,TIM_CHANNEL_4,0);

	}
}

#endif

static void motor_init(void)
{
	#if NOT_DMA_MOTOR
	HAL_TIM_PWM_Start(DshotTIM,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(DshotTIM,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(DshotTIM,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(DshotTIM,TIM_CHANNEL_4);	
	
	#endif
}

//Çý¶¯¹ÒÔØ
MotorInterface_t UserDshotMotor = {
	.init = motor_init,
	.set_target = pwmWriteDigital,
};
