#include "stm32f10x.h"                  // Device header
#include "OLED.h"

void StartTable(void)
{
	OLED_ShowString(1 ,2 ,"LED_Cotrol");
	OLED_ShowString(2 ,2 ,"PID");
	OLED_ShowString(3 ,2 ,"Image");
	OLED_ShowString(4 ,2 ,"Angle");
	OLED_ShowChar(1 ,1 ,'>');
}

void Point_move(uint8_t address)
{
	if(address == 1)
	{
		OLED_ShowChar(1 ,1 ,'>');
		OLED_ShowChar(2 ,1 ,' ');
		OLED_ShowChar(3 ,1 ,' ');
		OLED_ShowChar(4 ,1 ,' ');
	}
	
	if(address == 2)
	{
		OLED_ShowChar(1 ,1 ,' ');
		OLED_ShowChar(2 ,1 ,'>');
		OLED_ShowChar(3 ,1 ,' ');
		OLED_ShowChar(4 ,1 ,' ');
	}
	
	if(address == 3)
	{
		OLED_ShowChar(1 ,1 ,' ');
		OLED_ShowChar(2 ,1 ,' ');
		OLED_ShowChar(3 ,1 ,'>');
		OLED_ShowChar(4 ,1 ,' ');
	}
	
	if(address == 4)
	{
		OLED_ShowChar(1 ,1 ,' ');
		OLED_ShowChar(2 ,1 ,' ');
		OLED_ShowChar(3 ,1 ,' ');
		OLED_ShowChar(4 ,1 ,'>');
	}
}

struct TableMoble
{
	char Line_One[16];
	char Line_Two[16];
	char Line_Three[16];
	char Line_Four[16];
};

struct TableMoble table1 ,table2 ,table3 ,table4;


