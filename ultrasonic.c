#include "device_driver.h"
/*
// Servo PWM : TIM4_CH3(PB8)
// Both Ultrasonic sensor TRIG : TIM4_CH4(PB9)
// Front Ultrasonic sensor ECHO Rising & Falling: TIM3_CH1(PA6)
// Front Ultrasonic sensor ECHO Rising & Falling: TIM3_CH2(PA7)
// TRIG와 ECHO의 TIM이 다름 주의
*/

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

void USONIC_TRIG_Init(void)
{
    //extled.c 의 TailLED_Init()에서 TIM4 init
    Macro_Write_Block(GPIOB->CRH, 0xf, 0xa, 4);     // CH4(PB9)
    // 출력 없음
    TIM4->CCR4 = TIM4->ARR;
    Macro_Write_Block(TIM4->CCMR2, 0xff, 0x68, 8);
    // OUT pin : active high, output enable
	Macro_Write_Block(TIM4->CCER, 0x3, 0x3, 12);

}

void USONIC_ECHO_Init(void)
{
    Macro_Set_Bit(RCC->APB1ENR, 1); // TIM3 EN
	Macro_Set_Bit(RCC->APB2ENR, 2); // GPIOA EN
	Macro_Set_Bit(RCC->APB2ENR, 0); // AFIO EN
    Macro_Write_Block(GPIOA->CRL, 0xf, 0x4, 24);    // CH1, Input floating
    Macro_Write_Block(GPIOA->CRL, 0xf, 0x4, 28);    // CH2, Input floating
    // TIM2 CR1 설정: ARPE=0, down count, repeat mode
	TIM3->CR1 = (0<<7)|(1<<4)|(0<<3);
    // Timer 주파수가 TIM3_FREQ가 되도록 PSC 설정
	TIM3->PSC = (unsigned int)(TIMXCLK/(double)TIM3_FREQ + 0.5) - 1;
    // ARR 18ms(약3m) 초과시 거리 측정 불가로 판단 (구현필요하나, 현재 먼 거리는 측정 필요가 없음)
    TIM3->ARR = TIM3_UE_PERIOD / TIM3_TICK - 1;
    // ICxF : 0011, 8번 클럭동안 일정한 레벨이어야 인식
	// ICxPSC : 00, no PSC
    // CCxS : input
    TIM3->CCMR1 = 0x3131;
    // CCxP : 0, rising edge
    // CCxE : 1, enable
	Macro_Write_Block(TIM3->CCER, 0x3, 0x3, 0);
    // CCxP : 0, rising edge
    // CCxE : 1, enable
	Macro_Write_Block(TIM3->CCER, 0x3, 0x3, 4);

    // TIM3->SR CH1 Timer Capture Interrupt Pending Clear
    Macro_Clear_Bit(TIM3->SR, 1);
    // TIM3->SR CH2 Timer Capture Interrupt Pending Clear
    Macro_Clear_Bit(TIM3->SR, 2);
    // NVIC Interrupt Pending Clear => NVIC용 Macro 사용
    NVIC_ClearPendingIRQ(TIM3_IRQn);
    // TIM3->DIER TIM3 CH1,2 Capture/Compare Interrupt Enable
    Macro_Set_Area(TIM3->DIER, 0x3, 1);
    // NVIC Interrupt Enable => NVIC용 Macro 사용
    NVIC_EnableIRQ(TIM3_IRQn);

	// TIM3 start
	Macro_Set_Bit(TIM3->CR1, 0);
}