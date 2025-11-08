#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
#include "string.h"
#include "Encoder.h"
#include "Delay.h"
#include "Timer.h"
#include "Motor.h"

uint8_t KeyNum = 0;
uint8_t Table = 1;//为1时,进行第一个任务,即闭环控速,为2时,进行第二个任务,即位置同步
int16_t Speed_Ture;//实际速度值
int16_t Speed_Load;//目标速度
int16_t PWM;

extern int8_t RxPacketNum;//串口接收的数字
extern int8_t Serial_Rxtype;//串口接收的数据类型,0为字符,1为数字

float Target, Actual, Out;
float Kp = 0, Ki = 0, Kd = 0;
float Error0, Error1, Error2;

int main(void)
{
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	Timer_Init();
	Motor_Init();
	Key_Init();
	
	OLED_ShowString(1, 1, "Mode1");
	OLED_ShowString(2, 1, "Speed:");
	OLED_ShowSignedNum(2, 7, Speed_Ture, 3);
	OLED_ShowString(3, 1, "PWM:");
	OLED_ShowSignedNum(3, 5, PWM, 3);
	
	Motor_SetSpeed(PWM);
	
	while(1)
	{
		KeyNum = Key_GetNum();
		OLED_ShowSignedNum(2, 7, Speed_Ture, 3);
		OLED_ShowSignedNum(3, 5, PWM, 3);
		
		if(KeyNum == 1)
		{
			if(PWM == 0)
			{
				PWM = 10;
				Motor_SetSpeed(PWM);
			}
			else if(PWM != 0)
			{
				PWM = 0;
				Motor_SetSpeed(PWM);
			}
			KeyNum = 0;
			if(Table == 1)
			{
				Table = 2;
				OLED_ShowString(1, 1, "Mode1");
			}
			
			else if(Table == 2)
			{
				Table = 1;
				OLED_ShowString(1, 1, "Mode2");
			}
			
		}
		
		if(Table == 1&& Serial_RxFlag == 1)
		{
			OLED_ShowString(4, 1, "                ");
			OLED_ShowString(4, 1, Serial_RxPacket);
			
			OLED_ShowSignedNum(4, 5, Serial_GetNum(), 3);
			
			//调试用
			if(Serial_GetType() == 1)
			{
				PWM = Serial_GetNum();
				Motor_SetSpeed(PWM);
			}
			//调试用
			
			Serial_RxFlag = 0;
		}
		
	}
}

void TIM1_UP_IRQHandler(void)//通道3
{
	static int16_t Motor_count;
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Motor_count++;
		if(Motor_count > 10)
		{
			Motor_count = 0;
			Speed_Ture = Encoder_Get();
		}
		
		Key_Tick();
//		PID_Tick();//也是10s一次
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


