#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>

char Serial_RxPacket[100];
uint8_t Serial_RxFlag;

int8_t Serial_Rxtype = 0;//为0时表示文本,为1时表示数字
char Serial_RxNum1[10];//去除数字正负号后的数字
int8_t RxPacketNum = 0;
int8_t Serial_RxSign;

int8_t Permanent_RxPacketNum = 0;  // 新增的永久变量

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStruture;
	USART_InitStruture.USART_BaudRate = 9600;
	USART_InitStruture.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruture.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruture.USART_Parity = USART_Parity_No;
	USART_InitStruture.USART_StopBits = USART_StopBits_1;
	USART_InitStruture.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStruture);
	
	/* 中断部分初始化 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while ( USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for(i = 0;String[i] != '\0';i++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t x, uint32_t y)
{
	uint32_t Result = 1;
	while(y -- )
	{
		Result *= x;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for(i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);
		
		if(RxState == 0)
		{
			if(RxData == '@')
			{
				RxState = 1;
				Serial_Rxtype = 0;//输入类型
				pRxPacket = 0;
				RxPacketNum = 0;//重置串口输入的数字
			}
			else if(RxData == '$')
			{
				RxState = 1;
				Serial_Rxtype = 1;
				pRxPacket = 0;
				RxPacketNum = 0;
			}
		}
		else if(RxState == 1)
		{
			if(RxData == '\r')
			{
				RxState = 2;
			}
			else
			{
				if(Serial_Rxtype == 0)
				{
					Serial_RxPacket[pRxPacket] = RxData;
					pRxPacket++;
				}
				
				else if(Serial_Rxtype == 1)
				{
					Serial_RxPacket[pRxPacket] = RxData;
					if(pRxPacket == 0)
					{
						if(RxData == '+')
						{
							Serial_RxSign = 1;
						}
						else if(RxData == '-')
						{
							Serial_RxSign = -1;
						}
						else
						{
							Serial_RxNum1[pRxPacket] = Serial_RxPacket[pRxPacket] - '0';
							RxPacketNum = RxPacketNum * 10 +Serial_RxNum1[pRxPacket];
						}
					}
					
					pRxPacket++;
					
				}
			}
		}
		else if(RxState == 2)
		{
			if(RxData =='\n')
			{			
				RxState = 0;
				Serial_RxPacket[pRxPacket] = '\0';
				
				if(Serial_Rxtype == 1)
				{
					RxPacketNum *= Serial_RxSign;
					Permanent_RxPacketNum = RxPacketNum;  // 永久变量
				}
				
				Serial_RxFlag = 1;
			}
		}
		
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
		
}

int8_t Serial_GetNum(void)
{
	if(Serial_Rxtype == 1)
	{
		return Permanent_RxPacketNum;
	}
	return 0;
}

int8_t Serial_GetType(void)
{
	return Serial_Rxtype;
}
