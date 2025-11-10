#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "main.h"

// 채널
typedef enum { ULTRASONIC_FRONT=0, ULTRASONIC_RIGHT, ULTRASONIC_LEFT } Ultrasonic_Channel;

// 핀 정의
#define FRONT_TRIG_PORT GPIOB
#define FRONT_TRIG_PIN  GPIO_PIN_15
#define RIGHT_TRIG_PORT GPIOB
#define RIGHT_TRIG_PIN  GPIO_PIN_14
#define LEFT_TRIG_PORT  GPIOB
#define LEFT_TRIG_PIN   GPIO_PIN_13

void Ultrasonic_Init(void);
void Ultrasonic_TriggerAll(void);
extern volatile float distance_F;
extern volatile float distance_R;
extern volatile float distance_L;
void Ultrasonic_TimeoutCheck(void);


#endif
