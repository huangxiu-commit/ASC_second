#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;//电机1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	PWM_Init();
}

void Motor_SetSpeed(int8_t Speed)
{
	if(Speed > 100)
	{
		Speed = 100;
	}
	
	if(Speed < -100)
	{
		Speed = -100;
	}
	
	if(Speed >= 0)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		GPIO_ResetBits(GPIOB, GPIO_Pin_13);
		PWM_SetCompare3(Speed);
	}
	else
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);
		GPIO_SetBits(GPIOB, GPIO_Pin_13);
		PWM_SetCompare3(-Speed);
	}
}

//void Motor_Tick(void)
//{
//	extern int16_t LoadSpeed;
//	
//}

//double kp;
//double ki;
//double kd;

//uint16_t Pid(int16_t Error)//输入误差,输出占空比
//{
//	static int16_t Error0;//上一次的
//	static int16_t Error1;//新一次的
//	
//	static int16_t SumError;
//	
//	Error0 = Error1;
//	Error1 = Error;
//	
//	SumError += Error;
//	
//	int16_t out;
//	
//	out = kp * Error + ki * SumError + kd * ( Error1 - Error0);
//	
//	return out;
//}



