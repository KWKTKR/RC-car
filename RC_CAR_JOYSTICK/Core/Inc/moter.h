#include "main.h"

#ifndef INC_MOTER_H_
#define INC_MOTER_H_


#define LEFT_IN1_Port 	GPIOC
#define LEFT_IN1_Pin  	GPIO_PIN_0


#define RIGHT_IN2_Port 	GPIOC
#define RIGHT_IN2_Pin  	GPIO_PIN_1


void Motor_Forward(void);
void Motor_Backward(void);
void Motor_Left(void);
void Motor_Right(void);
void Motor_Stop(void);


#endif /* INC_MOTER_H_ */
