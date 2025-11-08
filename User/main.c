#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Serial.h"
#include "Key.h"
#include "string.h"
#include "Encoder.h"

int main(void)
{
	OLED_Init();
	Encoder_Init();
	Serial_Init();
	
	while(1)
	{
		
		
	}
}

void TIM4_IRQHandler(void)//通道3
{
	static uint16_t count;
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)
	{
		count++;
		if(count > 20)
		{
			count = 0;
			Key_Tick();
			//PID_Tick();
		}
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}


