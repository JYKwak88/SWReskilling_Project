/*
// External LED control
// Low : On, High : Off
// Head : PA4
// Rear : PA5 (후진)
// Tail : TIM4_CH2(PB7)
// Left : PA13
// Right : PA14
*/
#include "device_driver.h"

void H_R_LED_Init(void)
{
    // Head, Rear : GPIO
    Macro_Set_Bit(RCC->APB2ENR, 2);     // GPIOA Clock
	Macro_Write_Block(GPIOA->CRL, 0xff, 0x66, 16);   // PA4~5
	Macro_Set_Area(GPIOA->ODR, 0x3, 4);     // 초기 high로 off

    // Left, Right : GPIO
    Macro_Write_Block(AFIO->MAPR, 0x7, 0x4, 24);    // JTAG,SWD Disable
    Macro_Set_Bit(RCC->APB2ENR, 2);     // GPIOA Clock
	Macro_Write_Block(GPIOA->CRH, 0xff, 0x22, 20);   // PA13,14
    BOTH_LED_OFF;

    // Left, Right blinking : TIM4 Interrup handler
    // Left, Right LED TIM4에서 count 도달시 반전시킴

}
void HeadLED_On(void)
{
    Macro_Clear_Bit(GPIOA->ODR, 4);
    Draw_monoBMP(LIGHT_ON_X, LIGHT_ON_Y, 24, 18, LIGHT_ON_COLOR, Img_Lighton, 0);
}
void HeadLED_Off(void)
{
    Macro_Set_Bit(GPIOA->ODR, 4);
    Draw_monoBMP(LIGHT_ON_X, LIGHT_ON_Y, 24, 18, LIGHT_OFF_COLOR, Img_Lighton, 0);
}
void ReverseLED_On(void) { Macro_Clear_Bit(GPIOA->ODR, 5); }
void ReverseLED_Off(void) { Macro_Set_Bit(GPIOA->ODR, 5); }

void TailLED_Init(void)
{
    // Tail(미등 구현)
    Macro_Set_Bit(RCC->APB1ENR, 2); // TIM4 EN
	Macro_Set_Bit(RCC->APB2ENR, 3); // GPIOB EN
	Macro_Set_Bit(RCC->APB2ENR, 0); // AFIO EN
    Macro_Write_Block(GPIOB->CRL, 0xf, 0xa, 28);    // CH2, Output Compare Ch. Alternative function push-pull
    // TIM2 CR1 설정: ARPE=0, down count, repeat mode
	TIM4->CR1 = (0<<7)|(1<<4)|(0<<3);
    // Timer 주파수가 TIM4_FREQ가 되도록 PSC 설정
	TIM4->PSC = (unsigned int)(TIMXCLK/(double)TIM4_FREQ + 0.5) - 1;
    // PWM 주기 20ms
    // define으로 계산
    TIM4->ARR = (TIM4_UE_PERIOD / TIM4_TICK) -1;
    // 초기 출력(low)은 duty 100% (서있으므로)
    TIM4->CCR2 = 0;
    // OC2M : 110 (PWM1 mode)
	// OC2PE : CCR preload enable
    // CC2S : output
    Macro_Write_Block(TIM4->CCMR1, 0xff, 0x68, 8);
    // OUT pin : active high, output enable
	Macro_Write_Block(TIM4->CCER, 0x3, 0x1, 4);

   	// UG 이벤트 발생 => TIM4->SR의 UIF bit가 1로 변경됨 (업데이트 발생함)
	Macro_Set_Bit(TIM4->EGR,0);

    // TIM4->SR Timer Interrupt Pending Clear
    Macro_Clear_Bit(TIM4->SR, 0);
    // NVIC Interrupt Pending Clear => NVIC용 Macro 사용
    NVIC_ClearPendingIRQ(TIM4_IRQn);
    // TIM4->DIER TIM4 Interrupt Enable
    Macro_Set_Bit(TIM4->DIER, 0);
    // NVIC Interrupt Enable => NVIC용 Macro 사용
    NVIC_EnableIRQ(TIM4_IRQn);

	// TIM4 start
	Macro_Set_Bit(TIM4->CR1, 0);
}
void TailLED_On(void)
{
    TIM4->CCR2 = 0;     // duty 100%
}
void TailLED_Release(void)
{
    if (LIGHT_ON || (AUTO_LIGHT && NIGHT)) TIM4->CCR2 = TIM4->ARR * 7 / 8;     // duty 12.5%
    else TIM4->CCR2 = TIM4->ARR;     // duty 0%
}

void LED_Control(void)
{
    if (LIGHT_ON || (AUTO_LIGHT && NIGHT)) HeadLED_On();
    else HeadLED_Off();

    if (SPEED == 0) TailLED_On();
    else TailLED_Release();

    if (SPEED < 0) ReverseLED_On();
    else ReverseLED_Off();

    if (!EMERGENCY) BlinkLED_Control();
}

void BlinkLED_Control(void)
{
    if (DIRECTION == -1)        R_LED_OFF;
    else if (DIRECTION == 1)    L_LED_OFF;
    else if (DIRECTION == 0)    BOTH_LED_OFF;
}