#include "moter.h"
#include "tim.h"
#include "stdio.h"

extern TIM_HandleTypeDef htim1;

#define LEFT_IN1_GPIO GPIOA
#define LEFT_IN1_PIN  GPIO_PIN_4
#define LEFT_IN2_GPIO GPIOB
#define LEFT_IN2_PIN  GPIO_PIN_0

#define RIGHT_IN1_GPIO GPIOC
#define RIGHT_IN1_PIN  GPIO_PIN_1
#define RIGHT_IN2_GPIO GPIOC
#define RIGHT_IN2_PIN  GPIO_PIN_0

void moter_set(int left, int right)
{
    // 왼쪽 모터
    if(left >= 0)
    {
        HAL_GPIO_WritePin(LEFT_IN1_GPIO, LEFT_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(LEFT_IN2_GPIO, LEFT_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, left * 65535 / 100);
    }
    else
    {
        HAL_GPIO_WritePin(LEFT_IN1_GPIO, LEFT_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LEFT_IN2_GPIO, LEFT_IN2_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, -left * 65535 / 100);
    }

    // 오른쪽 모터
    if(right >= 0)
    {
        HAL_GPIO_WritePin(RIGHT_IN1_GPIO, RIGHT_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(RIGHT_IN2_GPIO, RIGHT_IN2_PIN, GPIO_PIN_RESET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, right * 65535 / 100);
    }
    else
    {
        HAL_GPIO_WritePin(RIGHT_IN1_GPIO, RIGHT_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(RIGHT_IN2_GPIO, RIGHT_IN2_PIN, GPIO_PIN_SET);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, -right * 65535 / 100);
    }

    // 디버깅 출력
    printf("Motor Left:%d Right:%d | L_IN1=%d L_IN2=%d R_IN1=%d R_IN2=%d\r\n",
           left, right,
           HAL_GPIO_ReadPin(LEFT_IN1_GPIO, LEFT_IN1_PIN),
           HAL_GPIO_ReadPin(LEFT_IN2_GPIO, LEFT_IN2_PIN),
           HAL_GPIO_ReadPin(RIGHT_IN1_GPIO, RIGHT_IN1_PIN),
           HAL_GPIO_ReadPin(RIGHT_IN2_GPIO, RIGHT_IN2_PIN));
}
