#include "car_control.h"
#include "moter.h"
#include "usart.h"
#include "tim.h"
#include "ultrasonic.h"
#include "stdio.h"
#include "string.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim11;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

volatile char rx_data;
volatile char mode = 'M';
volatile uint8_t auto_flag = 0;

// UART printf 지원
int fputc(int ch, FILE *f)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)&ch, 1, 10);
	return ch;
}

void RC_Init(void)
{
	HAL_TIM_Base_Start_IT(&htim11); // 100ms 타이머
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_data, 1);
	Ultrasonic_Init(); // TIM3 IC 시작
}

/*
 * 조이스틱 조종
 * 수동모드일때만
 */

void ManualDrive(char cmd)
{
	switch(cmd)
	{
	case 'F': moter_set(-60,-60); break;
	case 'B': moter_set(60,60); break;
	case 'L': moter_set(60,-60); break;
	case 'R': moter_set(-60,60); break;
	case 'S': moter_set(0,0); break;
	default: moter_set(0,0); break;
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM11)
		auto_flag = (mode == 'A') ? 1 : 0;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance != USART1) return;

	if(rx_data == 'A' || rx_data == 'M')
	{
		mode = rx_data;
		if(mode == 'M')
		{
			auto_flag = 0;			// 오토플러그 0
			moter_set(0,0);			// 멈춤 기본상태
		}
		// 디버깅
		char buf[64];
		snprintf(buf, sizeof(buf), "Mode changed: %c\r\n", mode);
		HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 10);
	}

	else if(mode == 'M')
		ManualDrive(rx_data);		//조이스틱 조종

	HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_data, 1);		//다음 수신 기달림
}


/*
 * 메인 while 문에있으며 모드가 A이거나 플러그가 1섰을때만 실행함수
 * A 모드 (자율 모드)일때 회피 함수
 */

void RC_Loop(void)
{
	if(mode != 'A' || !auto_flag) return;
	auto_flag = 0;

	Ultrasonic_TriggerAll();		// 트리거 3개
	Ultrasonic_TimeoutCheck();   	// Echo 미수신 대비

	float dist_front = distance_F;
	float dist_right = distance_R;
	float dist_left  = distance_L;

	const float SIDE_MIN = 5.3f;   // 옆 벽 최소 거리 기준 5.3 cm

	// ===========================================
	// 1) 좌/우 벽 너무 가까우면 → 후진 또는 회전(한 번만!)
	// ===========================================
	if(dist_left < SIDE_MIN)
	{
		// 왼쪽 벽 붙었음 → 오른쪽으로 피해야 함
		moter_set(40, 70);     // 한 번 후진
		return;                // 다음 루프에서 다시 판단
	}

	if(dist_right < SIDE_MIN)
	{
		// 오른쪽 벽 붙었음 → 왼쪽으로 피해야 함
		moter_set(70, 40);     // 한 번 후진
		return;                // 다음 루프에서 다시 판단
	}

	// ===========================================
	// 2) 전방 장애물 회피 (기존 유지)
	// ===========================================
	if(dist_front < 26.0f)
	{
		if(dist_left > dist_right)
			moter_set(-73, 73);     // 왼쪽 회전
		else
			moter_set(73, -73);     // 오른쪽 회전

		return;
	}

	// ===========================================
	// 3) 안전 → 직진
	// ===========================================
	moter_set(-55, -55);

	// 디버깅
	char buf[128];
	snprintf(buf, sizeof(buf),
			"Auto F=%.1f R=%.1f L=%.1f\r\n",
			dist_front, dist_right, dist_left);
	HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), 10);
}



