/*
// PC Wiring : UART1
// BT Wirelsse : UART3
*/

#include "device_driver.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

void Uart1_Init(int baud)
{
	double div;
	unsigned int mant;
	unsigned int frac;

	Macro_Set_Bit(RCC->APB2ENR, 2);
	Macro_Set_Bit(RCC->APB2ENR, 14);
	Macro_Write_Block(GPIOA->CRH, 0xff, 0x8a, 4);
	Macro_Set_Bit(GPIOA->ODR, 10);

	div = PCLK2/(16. * baud);
	mant = (int)div;
	frac = (int)((div - mant) * 16. + 0.5);
	mant += frac >> 4;
	frac &= 0xf;

	USART1->BRR = (mant<<4)+(frac<<0);
	USART1->CR1 = (1<<13)|(0<<12)|(0<<10)|(1<<3)|(1<<2);
	USART1->CR2 = 0<<12;
	USART1->CR3 = 0;

	// NVIC USART1 Pending clear
	NVIC_ClearPendingIRQ(USART1_IRQn);
	// USART1 RX interrupt enable
	Macro_Set_Bit(USART1->CR1, 5);
	// NVIC USART1 interrupt enable
	NVIC_EnableIRQ(USART1_IRQn);
}

void Uart1_Send_Byte(char data)
{
	if(data=='\n')
	{
		while(Macro_Check_Bit_Clear(USART1->SR, 7));
		USART1->DR = 0x0d;
	}

	while(Macro_Check_Bit_Clear(USART1->SR, 7));
	USART1->DR = data;
}

void Uart1_Send_String(char *pt)
{
	while(*pt!=0)
	{
		Uart1_Send_Byte(*pt++);
	}
}

void Uart1_Printf(char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Uart1_Send_String(string);
	va_end(ap);
}


void Uart3_Init(int baud)
{
	double div;
	unsigned int mant;
	unsigned int frac;

	Macro_Set_Bit(RCC->APB2ENR, 3);
	Macro_Set_Bit(RCC->APB1ENR, 18);
	Macro_Write_Block(GPIOB->CRH, 0xff, 0x8a, 8);
	Macro_Set_Bit(GPIOB->ODR, 11);

	div = PCLK1/(16. * baud);
	mant = (int)div;
	frac = (int)((div - mant) * 16. + 0.5);
	mant += frac >> 4;
	frac &= 0xf;

	USART3->BRR = (mant<<4)+(frac<<0);
	USART3->CR1 = (1<<13)|(0<<12)|(0<<10)|(1<<3)|(1<<2);
	USART3->CR2 = 0<<12;
	USART3->CR3 = 0;

	// NVIC USART1 Pending clear
	NVIC_ClearPendingIRQ(USART3_IRQn);
	// USART1 RX interrupt enable
	Macro_Set_Bit(USART3->CR1, 5);
	// NVIC USART1 interrupt enable
	NVIC_EnableIRQ(USART3_IRQn);
}

void Uart3_Send_Byte(char data)
{
	if(data=='\n')
	{
		while(Macro_Check_Bit_Clear(USART3->SR, 7));
		USART3->DR = 0x0d;
	}

	while(Macro_Check_Bit_Clear(USART3->SR, 7));
	USART3->DR = data;
}

void Uart3_Send_String(char *pt)
{
	while(*pt!=0)
	{
		Uart3_Send_Byte(*pt++);
	}
}

void Uart3_Printf(char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Uart3_Send_String(string);
	va_end(ap);
}
