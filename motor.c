/*
// TIM2 CH1~4 모두 모터 PWM 제어로 사용
//         CH1     CH2     CH3     CH4
// Fwd     H       L       H       L
// Rev     L       H       L       H
// Stop    L       L       L       L
*/

#include "device_driver.h"

void TIM2_Repeat(void)
{
	Macro_Set_Bit(RCC->APB1ENR, 0); // TIM2 EN
	Macro_Set_Bit(RCC->APB2ENR, 2); // GPIOA EN
	Macro_Set_Bit(RCC->APB2ENR, 0); // AFIO EN
    Macro_Write_Block(GPIOA->CRL, 0xffff, 0xaaaa, 0);    // CH1~4, Output Compare Ch. Alternative function push-pull
    // TIM2 CR1 설정: ARPE=0, down count, repeat mode
	TIM2->CR1 = (0<<7)|(1<<4)|(0<<3);
    // Timer 주파수가 TIM2_FREQ가 되도록 PSC 설정
	TIM2->PSC = (unsigned int)(TIMXCLK/(double)TIM2_FREQ + 0.5) - 1;
    // PWM 주기 10ms/5 = 2ms = 500Hz (최적화필요)
	// TIM2->ARR = TIM2_PLS_OF_10ms / 5 - 1;
    // define으로 계산
    TIM2->ARR = (TIM2_UE_PERIOD / TIM2_TICK) - 1;
    // 초기 출력은 duty 0%
    TIM2->CCR1 = TIM2->ARR;
    TIM2->CCR2 = TIM2->ARR;
    TIM2->CCR3 = TIM2->ARR;
    TIM2->CCR4 = TIM2->ARR;
    // OC1M : 110 (PWM1 mode)
	// Macro_Write_Block(TIM2->CCMR1, 0x7, 0x6, 4);
	// OC1PE : CCR preload enable
    // Macro_Set_Bit(TIM2->CCMR1, 3);
    // CC1S : output
	// Macro_Write_Block(TIM2->CCMR1, 0x3, 0x0, 0);
    TIM2->CCMR1 = 0x6868;
    TIM2->CCMR2 = 0x6868;
    // OUT pin : active low, output enable
	Macro_Write_Block(TIM2->CCER, 0x3, 0x3, 0);
	Macro_Write_Block(TIM2->CCER, 0x3, 0x3, 4);
	Macro_Write_Block(TIM2->CCER, 0x3, 0x3, 8);
	Macro_Write_Block(TIM2->CCER, 0x3, 0x3, 12);
   	// UG 이벤트 발생 => TIM2->SR의 UIF bit가 1로 변경됨 (업데이트 발생함)
	Macro_Set_Bit(TIM2->EGR,0);
	// TIM2 start
	Macro_Set_Bit(TIM2->CR1, 0);
}

void Motor_Init(void)
{
	TIM2_Repeat();
    Uart_Printf("========== Engine Start ==========\n\r\n\r");
}

struct _SPEED
{
    int lf, lr, rf, rr;
};
                            //spd  back2[-2]  back[-1]  stop[0]   one[1]    two[2]
const struct _SPEED gear[3][8] = {{{0,9,0,9},{0,3,0,5},{0,5,5,0},{0,0,5,0},{2,0,6,0},{3,0,7,0},{4,0,8,0},{5,0,10,0}},   // left[-1]
                                  {{0,9,0,9},{0,5,0,5},{0,0,0,0},{5,0,5,0},{6,0,6,0},{7,0,7,0},{8,0,8,0},{10,0,10,0}},  // center[0]
                                  {{0,9,0,9},{0,5,0,3},{5,0,0,5},{5,0,0,0},{6,0,2,0},{7,0,3,0},{8,0,4,0},{10,0,5,0}}};  // right[1]
void Motor_Drive(int dir, int spd)
{
    TIM2->CCR1 = TIM2->ARR * (10-gear[dir+1][spd+2].lf) / 10;
    TIM2->CCR2 = TIM2->ARR * (10-gear[dir+1][spd+2].lr) / 10;
    TIM2->CCR3 = TIM2->ARR * (10-gear[dir+1][spd+2].rf) / 10;
    TIM2->CCR4 = TIM2->ARR * (10-gear[dir+1][spd+2].rr) / 10;

    // 구조체 없이 계산식으로 만들기	
    // int lf=0, lr=0, rf=0, rr=0;

    // if (spd == 0)
    // {
    //     lf = lr = rf = rr = 0;
    // }
    // else if (spd > 0)
    // {
    //     lf = rf = spd + 5;
    //     lr = rr = 0;
    // }
    // else if (spd < 0)
    // {
    //     lf = rf = 0;
    //     lr = rr = -spd * 2 + 4;
    // }

    // if (dir == -1)
    // {
    //     //구현
    // }

}