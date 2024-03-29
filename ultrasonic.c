#include "device_driver.h"

void Servo_Init(void)
{
    //extled.c 의 TailLED_Init()에서 TIM3 init
    Macro_Write_Block(GPIOB->CRH, 0xf, 0xa, 0);     // CH3(PB8)
    // 초기 출력(high)은 duty 7.5% (center)
    // TIM4->CCR3 = TIM4->ARR * 7.5 / 100;
    SEE_CENTER;
    // OUT pin : active high, output enable
    Macro_Write_Block(TIM4->CCMR2, 0xff, 0x68, 0);
	Macro_Write_Block(TIM4->CCER, 0x3, 0x1, 8);
}