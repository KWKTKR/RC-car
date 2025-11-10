#include "ultrasonic.h"
#include "tim.h"

// 외부 TIM
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim10;

// 거리 변수
volatile float distance_F = 0;
volatile float distance_R = 0;
volatile float distance_L = 0;

// 캡처 변수
volatile uint32_t IC1_F=0, IC2_F=0;
volatile uint32_t IC1_R=0, IC2_R=0;
volatile uint32_t IC1_L=0, IC2_L=0;

volatile uint8_t capF=0, capR=0, capL=0;

void Ultrasonic_TimeoutCheck(void)
{
    if(capF == 1) { distance_F = 2.0f; capF = 0; }
    if(capR == 1) { distance_R = 2.0f; capR = 0; }
    if(capL == 1) { distance_L = 2.0f; capL = 0; }
}

void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim10,0);
    HAL_TIM_Base_Start(&htim10);
    while(__HAL_TIM_GET_COUNTER(&htim10)<us);
    HAL_TIM_Base_Stop(&htim10);
}

void Ultrasonic_Init(void)
{
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3);
}

// 트리거 한 번 발생
static void Trigger(GPIO_TypeDef* PORT, uint16_t PIN)
{
    HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);
    delay_us(2);
    HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_SET);
    delay_us(10);
    HAL_GPIO_WritePin(PORT, PIN, GPIO_PIN_RESET);
}

// 3채널 동시 Trigger
void Ultrasonic_TriggerAll(void)
{
    Trigger(FRONT_TRIG_PORT, FRONT_TRIG_PIN);
    delay_us(300);

    Trigger(RIGHT_TRIG_PORT, RIGHT_TRIG_PIN);
    delay_us(300);

    Trigger(LEFT_TRIG_PORT, LEFT_TRIG_PIN);
    delay_us(300);


}

// TIM IC 콜백 (htim3)
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // Front
    if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_1)
    {
        if(capF==0) { IC1_F=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1); capF=1; __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_FALLING);}
        else if(capF==1){ IC2_F=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_1);
            distance_F = (IC2_F>=IC1_F)? (IC2_F-IC1_F)/58.0f : ((0xFFFF-IC1_F)+IC2_F)/58.0f;
            capF=0; __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_1,TIM_INPUTCHANNELPOLARITY_RISING);}
    }
    // Right
    else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_2)
    {
        if(capR==0) { IC1_R=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2); capR=1; __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);}
        else if(capR==1){ IC2_R=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2);
            distance_R = (IC2_R>=IC1_R)? (IC2_R-IC1_R)/58.0f : ((0xFFFF-IC1_R)+IC2_R)/58.0f;
            capR=0; __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);}
    }
    // Left
    else if(htim->Channel==HAL_TIM_ACTIVE_CHANNEL_3)
    {
        if(capL==0) { IC1_L=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_3); capL=1; __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_3,TIM_INPUTCHANNELPOLARITY_FALLING);}
        else if(capL==1){ IC2_L=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_3);
            distance_L = (IC2_L>=IC1_L)? (IC2_L-IC1_L)/58.0f : ((0xFFFF-IC1_L)+IC2_L)/58.0f;
            capL=0; __HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_3,TIM_INPUTCHANNELPOLARITY_RISING);}
    }
}
