#include "stm32f10x.h"                  // Device header
#include "Delay.h"

uint8_t Key_Num;

/* 引脚配置 */
void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_GetState(void)
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0) == 0)
	{
		return 1;
	}
	return 0;
}

/* 获取按键返回 */
uint8_t Key_GetNum(void)
{
	uint8_t Temp;//临时变量,存储键码,返回后清除
	
	if(Key_Num != 0)
	{
		Temp = Key_Num;
		Key_Num = 0;
		return Temp;
	}
	
	return 0;
}

void Key_Tick(void)
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;
	
	Count++;
	if(Count >= 20)//每20ms检验一次按键状态
	{
		Count = 0;
		
		PrevState = CurrState;
		CurrState = Key_GetState();
		
		if(CurrState == 0 && PrevState != 0)
		{
			Key_Num = PrevState;
		}
	}
}
