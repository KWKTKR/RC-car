/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */

void HC05_SendATCommand(UART_HandleTypeDef *huart, const char *cmd);
void UART_ProcessCommand(void);

extern uint8_t txdata;


uint8_t rx_data;		// 기존: UART1 (HC-05) 수신 버퍼
uint8_t rx_data_u2;    //  추가: UART2 (PC 입력) 수신 버퍼


void UART_Init(void)
{
	HAL_UART_Receive_IT(&huart1, &rx_data, 1);
	HAL_UART_Receive_IT(&huart2, &rx_data_u2, 1); //
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1) // HC-05 → PC
    {
        HAL_UART_Transmit(&huart2, &rx_data, 1, 10);
        UART_ProcessCommand(); // 모터 명령 처리
        HAL_UART_Receive_IT(&huart1, &rx_data, 1);
    }
    else if (huart->Instance == USART2) // PC → HC-05
    {
        HAL_UART_Transmit(&huart1, &rx_data_u2, 1, 10);
        HAL_UART_Transmit(&huart2, &rx_data_u2, 1, 10); // PC Echo
        HAL_UART_Receive_IT(&huart2, &rx_data_u2, 1);
    }
}

void HC05_BindToHC06(void)
{
    HAL_Delay(1000); // 전원 직후 대기

    HC05_SendATCommand(&huart1, "AT");                   // AT 모드 확인
    HAL_Delay(500);

    HC05_SendATCommand(&huart1, "AT+BIND=98DA,60,0C9B59"); // HC-06 주소 바인딩
    HAL_Delay(500);

    HC05_SendATCommand(&huart1, "AT+RESET");            // 변경 사항 적용
    HAL_Delay(1000);
}


// HC-05 AT 명령 전송 함수
void HC05_SendATCommand(UART_HandleTypeDef *huart, const char *cmd)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%s\r\n", cmd);
    HAL_UART_Transmit(huart, (uint8_t*)buf, strlen(buf), 500);
}

//=================================================================

//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(huart->Instance == USART1)
//	{
//
//		// 1. HC-05의 응답을 PC (UART2)로 전송
//		HAL_UART_Transmit(&huart2, &rx_data, 1, 10);
//
//		// 2. 명령 처리 (AT 모드 설정 후 데이터 통신 모드에서 필요하므로 유지)&&&&&&&&&&&
//		// UART_ProcessCommand();
//
//		// 3. 다음 1바이트 수신 대기 시작 (필수!)
//		HAL_UART_Receive_IT(&huart1, &rx_data, 1);
//	}
//	else if(huart->Instance == USART2)
//	{
//		// 1. PC로부터 받은 데이터를 HC-05 (UART1)로 전송 (AT 명령 중계)
//		HAL_UART_Transmit(&huart1, &rx_data_u2, 1, 10);
//
//
//		// 2. PC 터미널로 Echo (사용자가 입력한 문자를 다시 화면에 표시)
//		HAL_UART_Transmit(&huart2, &rx_data_u2, 1, 10);
//
//
//		// 3. 다음 1바이트 수신 대기 시작 (필수!)
//		HAL_UART_Receive_IT(&huart2, &rx_data_u2, 1);
//	}
//}


void UART_ProcessCommand(void)
{
	switch(rx_data)
	{
	case 'F': Motor_Forward(); break;
	case 'B': Motor_Backward(); break;
	case 'L': Motor_Left(); break;
	case 'R': Motor_Right(); break;
	case 'S': Motor_Stop(); break;
	}
}



/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA10     ------> USART1_RX
    PA15     ------> USART1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA10     ------> USART1_RX
    PA15     ------> USART1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10|GPIO_PIN_15);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
