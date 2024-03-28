#include "device_driver.h"

void SysTick_Delay_ms(u32 msec)
{
	Macro_Clear_Area(SysTick->CTRL, 0x7, 0);
	SysTick->LOAD = (u32)((HCLK/(8.*1000.))*msec+0.5);
	SysTick->VAL = 0;
	Macro_Set_Bit(SysTick->CTRL, 0);

	while (Macro_Check_Bit_Clear(SysTick->CTRL, 16));

	SysTick->CTRL = 0;
}