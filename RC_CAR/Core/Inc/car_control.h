#ifndef __CAR_CONTROL_H__
#define __CAR_CONTROL_H__

#include "main.h"
#include "ultrasonic.h"

// 외부 TIM, UART 핸들러
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim11;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

// 모드 플래그
extern volatile char mode;      // 'M'=Manual, 'A'=Auto
extern volatile uint8_t auto_flag;

// 함수 선언
void RC_Init(void);
void RC_Loop(void);
void ManualDrive(char cmd);

#endif
