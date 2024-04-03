#include "device_driver.h"

void Buzzer_Init(void)
{
    Macro_Write_Block(AFIO->MAPR, 0x3, 0x1, 6);
    Macro_Set_Bit(RCC->APB2ENR, 11); // TIM1 EN
	Macro_Set_Bit(RCC->APB2ENR, 3); // GPIOB EN
	Macro_Set_Bit(RCC->APB2ENR, 0); // AFIO EN
    Macro_Write_Block(GPIOB->CRL, 0xf, 0xb, 0);    // CH2N(PB0), Output Compare Ch. Alternative function push-pull
    // OC2M : 110 (PWM1 mode)
    Macro_Write_Block(TIM1->CCMR1, 0x7, 0x6, 12);
	// OC2PE : CCR preload enable
    Macro_Set_Bit(TIM1->CCMR1, 11);
    // CC2S : output
    Macro_Write_Block(TIM1->CCMR1, 0x3, 0x0, 8);

    Macro_Clear_Area(TIM1->BDTR, 0x3, 8);   // LOCK = 00;
    Macro_Set_Bit(TIM1->BDTR, 15);  // MOE = 1
    Macro_Set_Bit(TIM1->BDTR, 14);  // AOE = 1
    Macro_Set_Bit(TIM1->BDTR, 11);  // OSSR = 1

    // Macro_Set_Area(TIM1->CCER, 0x3, 4); // CC2P = 1, CC2E = 1
    // OUT pin : active high, output enable
    Macro_Set_Area(TIM1->CCER, 0x3, 6);  // CC2NP = 1, CC2NE = 1

    // TIM1 CR1 설정: ARPE=0, down count, repeat mode
	TIM1->CR1 = (0<<7)|(1<<4)|(0<<3);
    // Timer 주파수가 TIM1_FREQ가 되도록 PSC 설정
	TIM1->PSC = (unsigned int)(TIMXCLK/(double)TIM1_FREQ + 0.5) - 1;

    // 440Hz 소리
    TIM1->ARR = (unsigned short)((double)TIM1_FREQ / 440 + 0.5) - 1;
    TIM1->CCR2 = TIM1->ARR / 2;

   	// UG 이벤트 발생 => TIM1->SR의 UIF bit가 1로 변경됨 (업데이트 발생함)
	// Macro_Set_Bit(TIM1->EGR,0);

    // TIM1->SR Timer Interrupt Pending Clear
    Macro_Clear_Bit(TIM1->SR, 0);
    // NVIC Interrupt Pending Clear => NVIC용 Macro 사용
    NVIC_ClearPendingIRQ(TIM1_UP_IRQn);
    // TIM1->DIER TIM1 Interrupt Enable
    // Macro_Set_Bit(TIM1->DIER, 0);
    // NVIC Interrupt Enable => NVIC용 Macro 사용
    NVIC_EnableIRQ(TIM1_UP_IRQn);

	// TIM1 start
	// Macro_Set_Bit(TIM1->CR1, 0);
}

void Buzzer_On(void)
{
    Macro_Set_Bit(TIM1->CR1, 0);
}

void Buzzer_Off(void)
{
    Macro_Clear_Bit(TIM1->CR1, 0);
}

void Buzzer_Changefreq(u16 freq)
{
    TIM1->ARR = (unsigned short)((double)TIM1_FREQ / freq + 0.5) - 1;
	Macro_Set_Bit(TIM1->EGR, 0);
}