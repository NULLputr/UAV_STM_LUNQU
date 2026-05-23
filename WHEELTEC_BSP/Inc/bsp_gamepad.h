#ifndef __BSP_GAMEPAD_H_
#define __BSP_GAMEPAD_H_

#include <stdint.h>

typedef enum{
	GamePadKeyEvent_NONE,
	GamePadKeyEvent_SINGLECLICK,
	GamePadKeyEvent_DOUBLECLICK,
	GamePadKeyEvent_LONGCLICK
}GamePadKeyEventType_t;


typedef enum{
	GamePadKeyState_Release,
	GamePadKeyState_Press
}GamePadKeyStateType_t;


typedef enum{
    WaitToPress = 0,
    WaitToRelease ,
    KEYPress    ,
    KEYUp       ,
    LONG_CLICK  ,
}GamePad_StateMachineType_t;

//通用手柄单个按键属性
typedef struct 
{
    uint8_t keystate;
    uint32_t timebase;
    uint32_t statetime;
    GamePad_StateMachineType_t statemachine;
}GamePad_CheckEventType_t;

//游戏手柄按键通用检测时间,单位ms
#define GamePad_LONGPRESS_TIEM 1000 //长按检测时间
#define GamePad_CLICK_TIME     400  //单双击检测时间
#define GamePad_KEYFILTER_TIME 50   //按键滤波时间



typedef struct{
	uint8_t LX;
	uint8_t LY;
	uint8_t RX;
	uint8_t RY;
	uint8_t LT;
	uint8_t RT;
	void (*SetVibration)(uint8_t m1,uint8_t m2);
	GamePadKeyStateType_t (*getKeyState)(uint8_t keybit);
	uint8_t StartFlag;
	uint8_t Vib_EN;
}GamePadType_t;


//游戏手柄品牌
typedef enum{
	UnKnown_Dev,          //未知设备
	PS2_USB_Wiredless1,    //无线ps2手柄
	PS2_USB_Wiredless2,    //无线ps2手柄
}GamePadBrandType_t;

enum {
	EnumNULL = 0,
	EnumWait,
	EnumDone
};

typedef struct{
	uint8_t ready;
	uint8_t enum_state;
	GamePadBrandType_t type;
}GamePadDebugValType_t;

//对游戏手柄提供按键检测的接口函数
GamePadKeyEventType_t GamePadKey_CheckEvent(uint16_t keysource,GamePad_CheckEventType_t* key,uint8_t bit);

//对外提供游戏手柄品牌参数
extern GamePadBrandType_t gamepad_brand;

//手柄设备插入与拔出回调函数
void USB_GamePad_InsertCallback(void);
void USB_GamePad_PullOutCallback(void);

//通用游戏手柄接口
extern GamePadType_t* GamePadInterface;
extern GamePadType_t GamePadDefalut;

//通用手柄调试接口
extern GamePadDebugValType_t GamePadDebug;

#endif


