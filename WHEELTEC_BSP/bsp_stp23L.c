#include "bsp_stp23L.h"

#include <stdio.h>
#include <string.h>

#include "bsp_RTOSdebug.h"

//创建一个存放STP23L数据的结构体对象
OriData_STP23L_t DMABuf_oridata_stp23L;

//STP23L高度数据
float g_readonly_distance = 0;

//创建一个stp23L模块对象
static STP23LSensorTypedef stp23l_sensor;
static const uint8_t stp23l_bufferlen = sizeof(STP23LSensorTypedef);
static uint8_t stp23l_buffer[stp23l_bufferlen];

//
//static pRtosDebugInterface_t debug = &RTOSTaskDebug;
//static RtosDebugPrivateVar priv = { 0 };

uint8_t stp23L_callback(OriData_STP23L_t* buffer)
{
	uint8_t ready = 0;
	
	//辅助变量
	static uint8_t last_recv = 0;
	static uint8_t TmpCnt = 0;
	static uint8_t datacount = 0;
	static uint8_t crc = 0;
	
	//状态机的状态内容
	enum{
		WaitHead = 0, //接收帧头
		WaitDevAddr,
		WaitCmdCode,
		WaitDiffAddr,
		WaitDataLen,
		WaitDATA,
		CRCCheck
	};
	
	//状态机
	static uint8_t state_m = WaitHead;
	
	//处理数据
	for(uint8_t i=0;i<buffer->DataLen;i++)
	{
		switch( state_m )
		{
			case WaitHead:
				if( buffer->Buf[i]==0xAA &&last_recv==0xAA ) TmpCnt++;
				else TmpCnt = 0;
				//成功收到帧头
			  if( TmpCnt==3 )
				{
					stp23l_buffer[0]=0xAA;stp23l_buffer[1]=0xAA;
					stp23l_buffer[2]=0xAA;stp23l_buffer[3]=0xAA;
					TmpCnt=0;
					state_m = WaitDevAddr;
					datacount=4;
				}					
				break;
				
			case WaitDevAddr:
				stp23l_buffer[datacount++]=buffer->Buf[i];
				crc+=buffer->Buf[i];
				state_m = WaitCmdCode;
				break;
			
			case WaitCmdCode:
				stp23l_buffer[datacount++]=buffer->Buf[i];
				crc+=buffer->Buf[i];
				state_m = WaitDiffAddr;
				break;
			
			case WaitDiffAddr:
				stp23l_buffer[datacount++]=buffer->Buf[i];
				crc+=buffer->Buf[i];
				TmpCnt++;
				if( TmpCnt==2 )
				{
					TmpCnt=0;
					state_m = WaitDataLen;
				}
				break;
				
			case WaitDataLen:
				stp23l_buffer[datacount++]=buffer->Buf[i];
				crc+=buffer->Buf[i];
				TmpCnt++;
				if( TmpCnt==2 )
				{
					TmpCnt=0;
					state_m = WaitDATA;
				}
				break;
			
			case WaitDATA:
				stp23l_buffer[datacount++]=buffer->Buf[i];
				crc+=buffer->Buf[i];
				TmpCnt++;
				if( TmpCnt==184 )
				{
					TmpCnt=0;
					state_m = CRCCheck;
				}
				break;
				
			case CRCCheck:
				stp23l_buffer[datacount++]=buffer->Buf[i];
				if( crc == buffer->Buf[i] )
				{
					//校验通过,执行数据解析
					memcpy((uint8_t*)&stp23l_sensor,stp23l_buffer,stp23l_bufferlen);
					
					float dis=0;
					for(uint8_t k=0;k<12;k++)
						dis += stp23l_sensor.point[k].distance;
					
					//取平均值并将单位转为 m （原始单位是mm）
					g_readonly_distance = dis/12000.0f;
					
					//数据就绪
					ready=1;
					
//					uint16_t freq=0;
//					freq = debug->UpdateFreq(&priv);
//					printf("freq:%d , distance=%.3f\r\n",freq,g_readonly_distance);
				}
				
				//复位所有变量
				TmpCnt=0;
				datacount=0;
				crc=0;
				state_m = WaitHead;
				
			default:
				break;
		}
		
		last_recv = buffer->Buf[i];
	}
	
	return ready;
}


