#include "joystick.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern ADC_HandleTypeDef hadc1;	  // 조이스틱 ADC 값
extern UART_HandleTypeDef huart1; // HC-05 (RC카 전송)
extern UART_HandleTypeDef huart2; // 디버깅용

#define CENTER_X 2048			  // X
#define CENTER_Y 2048			  // Y
#define DEADZONE 200		      // 데드존

char mode = 'M'; // 기본 수동 모드

/* UART로 명령 전송 */
void sendcommand(char cmd)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&cmd, 1, 10);
}

/* ADC 읽기 함수 */
uint32_t readADC(uint32_t channel)
{
	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
	return HAL_ADC_GetValue(&hadc1);
}

/* 조이스틱 루프 */
void loopJoystick(void)
{
	static uint8_t prevSWBState = 0;
	uint8_t currSWBState = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4);

	// SWB 버튼 → M/A 모드 토글
	if(currSWBState == GPIO_PIN_SET && prevSWBState == 0)
	{
		mode = (mode == 'A') ? 'M' : 'A';

		char buf[32];
		int len = snprintf(buf, sizeof(buf), "Mode changed: %c\r\n", mode);
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, len, 10);

		// RC카에 모드 전송
		sendcommand(mode);
	}
	prevSWBState = currSWBState;

	if(mode == 'A') return; // 자율 모드면 리턴

	// 수동 모드일 때 조이스틱 읽기
	uint32_t x = readADC(ADC_CHANNEL_0);
	uint32_t y = readADC(ADC_CHANNEL_1);

	int xdiff = (int)x - CENTER_X;
	int ydiff = (int)y - CENTER_Y;

	if(abs(xdiff) < DEADZONE) xdiff = 0;
	if(abs(ydiff) < DEADZONE) ydiff = 0;

	char cmd = 'S'; // 기본 정지
	if(ydiff > 0 && abs(ydiff) > abs(xdiff)) cmd = 'F';       // Forward
	else if(ydiff < 0 && abs(ydiff) > abs(xdiff)) cmd = 'B';  // Backward
	else if(xdiff > 0 && abs(xdiff) > abs(ydiff)) cmd = 'R';  // Right
	else if(xdiff < 0 && abs(xdiff) > abs(ydiff)) cmd = 'L';  // Left

	sendcommand(cmd);	//유아트 전송

	//디버깅
	char buf[64];
	int len = snprintf(buf, sizeof(buf), "X=%lu Y=%lu CMD=%c\r\n", x, y, cmd);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, len, 10);

	HAL_Delay(100); // 100ms 주기
}
