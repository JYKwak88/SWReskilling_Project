#include "device_driver.h"

void CDS_Init(void)
{
	// ADC_IN9 Configuration
	Macro_Set_Bit(RCC->APB2ENR, 3); 				// PB POWER ON
	Macro_Write_Block(GPIOA->CRL, 0xf, 0x0, 4); 	// PB1(ADC-IN9) = Analog Input

	Macro_Set_Bit(RCC->APB2ENR, 9); 				// ADC1 POWER ON
	Macro_Write_Block(RCC->CFGR, 0x3, 0x2, 14); 	// ADC1 CLOCK = 12MHz(PCLK2/6) : divide by 00=2, 01=4, 10=6, 11=8

	Macro_Write_Block(ADC1->SMPR2, 0x7, 0x7, 27); 	// Sampling rate of CH9 = 239.5 Cycles
	Macro_Write_Block(ADC1->SQR1, 0xF, 0x0, 20); 	// Conversion Sequence No = 1
	Macro_Write_Block(ADC1->SQR3, 0x1F, 9, 0); 		// Sequence Channel of No 1 = CH9
	Macro_Write_Block(ADC1->CR2, 0x7, 0x7, 17); 	// EXT Trigger = SW Trigger
    // Macro_Set_Bit(ADC1->CR2, 1);                    // Continuous


    Macro_Clear_Bit(ADC1->SR, 1);                   // EOC Interrupt clear
    NVIC_ClearPendingIRQ(ADC1_2_IRQn);              // NVIC ADC Interrupt Pending clear

    Macro_Set_Bit(ADC1->CR1, 5);                    // EOC Interrupt EN
    NVIC_EnableIRQ(ADC1_2_IRQn);                    // NVIC ADC Interrupt EN

	Macro_Set_Bit(ADC1->CR2, 0); 					// ADC ON

    CDS_Start();
}

void CDS_Start(void)
{
	Macro_Set_Bit(ADC1->CR2, 20); 					// EXT Trigger Start
	Macro_Set_Bit(ADC1->CR2, 22); 					// ADC Start
}

void CDS_Stop(void)
{
	Macro_Clear_Bit(ADC1->CR2, 20); 				// EXT Trigger Stop
	Macro_Clear_Bit(ADC1->CR2, 22); 				// ADC Stop
}
