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

char MingLing1[7] = "speed%";
char MingLing2[11];

float Target, Actual, Out;
float Kp = 0.2, Ki = 0.2, Kd = 0;//初始默认任务一的
float Error0, Error1, Error2;
float ErrorInt;

int main(void)
{
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	Timer_Init();
	Motor_Init();
	Key_Init();
	
//	OLED_ShowString(1, 1, "Mode1");
	OLED_ShowString(1, 1, "Tar:");
	OLED_ShowSignedNum(1, 4, Target, 3);
	OLED_ShowString(2, 1, "Act:");
	OLED_ShowSignedNum(2, 4, Actual, 3);
	OLED_ShowString(3, 1, "Out:");
	OLED_ShowSignedNum(3, 4, Out, 3);
	
	while(1)
	{
		KeyNum = Key_GetNum();
		OLED_ShowSignedNum(1, 4, Target, 3);
		OLED_ShowSignedNum(2, 4, Actual, 3);
		OLED_ShowSignedNum(3, 4, Out, 3);
		
		if(KeyNum == 1)
		{
			KeyNum = 0;
			Motor_SetSpeed(10);
			
			if(Target <= 20)
			{
				Target += 10;
			}
			//调试用
			
			if(Table == 1)
			{
				Table = 2;
				OLED_ShowString(1, 1, "Mode1");
				Error0 = 0;//清除之前留下的
				Error1 = 0;
				Error2 = 0;
				ErrorInt = 0;
				Kp = 0;//变化为调好的预定值
				Ki = 0;
				Kd = 0;
			}
			
			else if(Table == 2)
			{
				Table = 1;
				OLED_ShowString(1, 1, "Mode2");
				Error0 = 0;
				Error1 = 0;
				Error2 = 0;
				ErrorInt = 0;
				Kp = 0;//变化为调好的预定值
				Ki = 0;
				Kd = 0;
			}
			
		}
		
		
		//存在问题:数字处理有误
		if(Table == 1&& Serial_RxFlag == 1)
		{
			OLED_ShowString(4, 1, "                ");
			OLED_ShowString(4, 1, Serial_RxPacket);
			
			uint8_t found=1;//若为0则表示不是指令Speed
			uint8_t TempNum = 0;
			
			for(int i=0;i<6;i++)
			{
				if(Serial_RxPacket[i] != MingLing1[i])
				{
					found = 0;
				}
			}
			
			if(found == 1)
			{
				for(int i =7;i<=9;i++)
				{
					TempNum = TempNum * 10 + (Serial_RxPacket[i] - '0');
				}
				
				Target = TempNum;
			}
			
			
			
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
		if(Motor_count >= 10)
		{
			Motor_count = 0;
			
			if(Table == 1)
			{
				Actual = Encoder_Get();
				
				Error2 = Error1;
				Error1 = Error0;
				Error0 = Target - Actual;
				
				Out += Kp * (Error0 - Error1) + Ki * Error0 + Kd * (Error0 - 2 * Error1 + Error2);
				
				if(Out > 100){Out = 100;}
				if(Out < -100){Out = -100;}
				
				Motor_SetSpeed(Out);
			}
			
			else if(Table == 2)
			{
				Actual += Encoder_Get2();
				
				Error1 = Error0;
				Error0 = Target - Actual;
				
				ErrorInt += Error0;
				
				Out = Kp * Error0 + Ki * ErrorInt + Kd * (Error0 - Error1);
				
				if(Out > 100){Out = 100;}
				if(Out < -100){Out = -100;}
				
				Motor_SetSpeed(Out);
			}
		}
		
		Key_Tick();
		
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}


