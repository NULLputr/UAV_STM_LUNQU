#include "bsp_gamepad.h"

#include "FreeRTOS.h"
#include "task.h"

GamePadDebugValType_t GamePadDebug = {0};

GamePadBrandType_t gamepad_brand = UnKnown_Dev;


void USB_GamePad_InsertCallback(void)
{
	GamePadDebug.type = gamepad_brand;
}

void USB_GamePad_PullOutCallback(void)
{
	gamepad_brand = UnKnown_Dev;
	
	GamePadInterface->LX = 127;
	GamePadInterface->RX = 127;
	
	GamePadInterface->LY = 127;
	GamePadInterface->RY = 127;
	
	GamePadInterface->LT = 0;
	GamePadInterface->RT = 0;
	
	GamePadInterface = &GamePadDefalut;
	
	GamePadDebug.enum_state = EnumNULL;
	GamePadDebug.type = gamepad_brand;
	GamePadDebug.ready = 0;
}


GamePadType_t GamePadDefalut = {
	.LX = 127,
	.LY = 127,
	.RX = 127,
	.RY = 127,
	.LT = 0,
	.RT = 0,
	.StartFlag = 0,
	.Vib_EN = 1,
	.SetVibration = 0,
	.getKeyState = 0
};

//通用游戏手柄对象
GamePadType_t* GamePadInterface = &GamePadDefalut;


//入口参数：按键原始值，按键属性，需要检测的按键
GamePadKeyEventType_t GamePadKey_CheckEvent(uint16_t keysource,GamePad_CheckEventType_t *key,uint8_t bit)
{
	key->keystate = (keysource >> bit)&0x01;
	
	switch (key->statemachine)
	{
		case WaitToPress:
			if(GamePadKeyState_Press == key->keystate)
			{
				key->timebase = xTaskGetTickCount();
				key->statemachine = KEYPress;
			}
			break;
			
		case KEYPress:
			key->statetime = xTaskGetTickCount() - key->timebase;
			
			if(GamePadKeyState_Release == key->keystate)
			{
				if(key->statetime < GamePad_KEYFILTER_TIME)
				{
					key->statemachine = WaitToPress;
				}
				else{
					key->timebase = xTaskGetTickCount();
					key->statemachine = KEYUp;
				}
			}
			else if(key->statetime > GamePad_LONGPRESS_TIEM)
			{
				key->statemachine = LONG_CLICK;
			}
			break;
			
		case KEYUp:
			key->statetime = xTaskGetTickCount() - key->timebase;
		
			if(GamePadKeyState_Press == key->keystate && key->statetime < GamePad_CLICK_TIME && key->statetime > GamePad_KEYFILTER_TIME)
			{
				key->statemachine = WaitToRelease;
				return GamePadKeyEvent_DOUBLECLICK;
			}
			else if(key->statetime >= GamePad_CLICK_TIME)
			{
				key->statemachine = WaitToRelease;
				return GamePadKeyEvent_SINGLECLICK;
			}
		break;
			
		case LONG_CLICK:
			key->statemachine = WaitToRelease;
			return GamePadKeyEvent_LONGCLICK;
		
		case WaitToRelease:
			if(GamePadKeyState_Release == key->keystate)
			{
				key->statemachine = WaitToPress;
			}
			break;
	}
	return GamePadKeyEvent_NONE;
	
}






















