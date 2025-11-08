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
int16_t Speed;//实际速度值


int main(void)
{
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	Timer_Init();
	Motor_Init();
	Key_Init();
	
	OLED_ShowString(3, 1, "Mode1");
	Motor_SetSpeed(-10);
	
	
	while(1)
	{
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			KeyNum = 0;
			Motor_SetSpeed(10);
			if(Table == 1)
			{
				Table = 2;
				OLED_ShowString(3, 1, "Mode1");
			}
			
			if(Table == 2)
			{
				Table = 1;
				OLED_ShowString(3, 1, "Mode2");
			}
			
		}
		
	}
}

void TIM1_UP_IRQHandler(void)//通道3
{
	static int16_t Motor_count;
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		Motor_count++;
		if(Motor_count > 100)
		{
			Motor_count = 0;
			OLED_ShowSignedNum(2, 1, Encoder_Get(), 5);
		}
			Key_Tick();
			
//			PID_Tick();
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


