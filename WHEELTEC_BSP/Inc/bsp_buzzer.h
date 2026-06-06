#ifndef __BSP_BUZZER_H_
#define __BSP_BUZZER_H_

typedef struct{
	void (*on)(void);
	void (*off)(void);
	void (*toggle)(void);
}BuzzerInterface_t,*pBuzzerInterface_t;

extern BuzzerInterface_t UserBuzzer;

#endif
