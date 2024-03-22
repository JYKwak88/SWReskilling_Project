#include "device_driver.h"
#include "lcd.h"
/****************************************************************************************************
// SCK          SPI2_SCK(PB13)
// SDI(MOSI)    SPI2_MOSI(PB15)
// SDO(MISO)    SPI2_MISO(PB14)
// CS           SPI2_NSS(PB12)
// Back Light   TIM1_CH1(PA8)
// DC/RS        PA6
// RST          PA7
**************************************************************************************************/	
#define SPI2_BR_OP (4)
// SPI2_BR_OP   PCLK Dividing   Baud Rate(MHz)   (SPI2:APB1 => PCLK1 = 36MHz)
// 0            2               18
// 1            4               9
// 2            8               4.5
// 3            16              2.25
// 4            32              1.125
// 5            64              ...
// 6            128
// 7            256

//중요 LCD 매개변수 관리
//기본값은 세로 화면입니다.
_lcd_dev lcddev;

//브러시 색상, 배경색
u16 POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
u16 DeviceCode;	 

void SPI2_Init(void)
{ 
	Macro_Set_Bit(RCC->APB2ENR, 3);       // PB Enable
	Macro_Write_Block(GPIOB->CRH, 0xffff, 0xb8b3, 16);  // PB12~15 : SPI 설정
	Macro_Set_Bit(GPIOB->ODR, 12);      // NSS High
	// Macro_Set_Bit(GPIOB->ODR, 14);      // MISO High

	Macro_Set_Bit(RCC->APB1ENR, 14);    // SPI2 Enable
	SPI2->CR1 = (0<<11)|(0<<10)|(1<<9)|(1<<8)|(0<<7)|(SPI2_BR_OP<<3)|(1<<2)|(0<<1)|(0<<0);
    // Bit:Name
    // 11:DFF(Data frame format)        0, 8-bit
    // 10:RXONLY                        0, Full duplex (Tx&Rx)
    //  9:SSM(SW slave manage)          1, Enable
    //  8:SSI(Internal slave select)    0, this bit is forced on to NSS pin // 실습예제는 1
    //  7:LSBFIRST(frame format)        0, MSB first
    //5-3:BR[2:0](Baud rate)            PCLK / 2^(value+1)       if freq=32 => PCLK(36MHz) / 32 = 1.125MHz
    //  2:MSTR(Master select)           1, Master
    //  1:CPOL(Clock polarity)          0, CK to 0 when idle
    //  0:CPHA(Clock phase)             0, first clk transistion is the first data capture edge
    //  ** CPOL,CPHA follows slave deivce SPI spec.
	Macro_Set_Bit(SPI2->CR1, 6);
    //  6:SPE(SPI enable)
}


void LCD_GPIO_Init(void)
{
// DC/RS        PA6
// RST          PA7
	Macro_Set_Area(RCC->APB2ENR, 0x3, 2);       // PA PB Enable
    Macro_Write_Block(GPIOA->CRL, 0xf, 0x3, 28);            // PA7 GPO P-P 50MHz
    Macro_Write_Block(GPIOA->CRL, 0xf, 0x3, 24);            // PA6 GPO P-P 50MHz
    // Macro_Write_Block(GPIOB->CRL, 0xf, 0x3, 20);            // PB5 GPO P-P 50MHz
}

void BLU_PWM_Init(void)
{
    Macro_Set_Bit(RCC->APB2ENR, 11); // TIM1 EN
	Macro_Set_Bit(RCC->APB2ENR, 2); // GPIOA EN
	Macro_Set_Bit(RCC->APB2ENR, 0); // AFIO EN
    Macro_Write_Block(GPIOA->CRH, 0xf, 0xa, 12);    // CH4(PA11), Output Compare Ch. Alternative function push-pull
    // TIM1 CR1 설정: ARPE=0, down count, repeat mode
	TIM1->CR1 = (0<<7)|(1<<4)|(0<<3);
    // Timer 주파수가 TIM1_FREQ가 되도록 PSC 설정
	TIM1->PSC = (unsigned int)((TIMXCLK/2)/(double)TIM1_FREQ + 0.5) - 1;
    // PWM 주기 = TIM1_UE_PERIOD
    TIM1->ARR = (TIM1_UE_PERIOD / TIM1_TICK) -1;
    // 초기 출력은 duty 50% test단계에서. 나중에 적절히 조절
    TIM1->CCR4 = TIM1->ARR / 2;
    // OC1M : 110 (PWM1 mode)
	// Macro_Write_Block(TIM1->CCMR1, 0x7, 0x6, 12);
	// OC1PE : CCR preload enable
    // Macro_Set_Bit(TIM1->CCMR1, 11);
    // CC1S : output
	// Macro_Write_Block(TIM1->CCMR1, 0x3, 0x0, 8);
    Macro_Write_Block(TIM1->CCMR2, 0xff, 0x68, 8);
    // OUT pin : active low, output enable
	Macro_Write_Block(TIM1->CCER, 0x3, 0x3, 12);

   	// UG 이벤트 발생 => TIM1->SR의 UIF bit가 1로 변경됨 (업데이트 발생함)
	Macro_Set_Bit(TIM1->EGR,0);

    // // TIM1->SR Timer Interrupt Pending Clear
    // Macro_Set_Bit(TIM1->SR, 0);
    // // NVIC Interrupt Pending Clear => NVIC용 Macro 사용
    // NVIC_ClearPendingIRQ(TIM1_UP_IRQn);
    // // TIM1->DIER TIM1 Interrupt Enable
    // Macro_Set_Bit(TIM1->DIER, 0);
    // // NVIC Interrupt Enable => NVIC용 Macro 사용
    // NVIC_EnableIRQ(TIM1_UP_IRQn);

	// TIM1 start
	Macro_Set_Bit(TIM1->CR1, 0);
}

void LCD_Reset(void)
{
    LCD_RST_CLR;
	SysTick_Delay_ms(100);
	LCD_RST_SET;
    SysTick_Delay_ms(50);
}

void LCD_WR_REG(u8 data)
{ 
    LCD_CS_CLR;
    LCD_RS_CLR;
    SPI_WriteByte(data);
    LCD_CS_SET;
}

void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{	
	LCD_WR_REG(LCD_Reg);  
	LCD_WR_DATA(LCD_RegValue);	    		 
}

void LCD_WR_DATA(u8 data)
{
    LCD_CS_CLR;
    LCD_RS_SET;
    SysTick_Delay_ms(20);
    SPI_WriteByte(data);
    SysTick_Delay_ms(20);
    LCD_CS_SET;
}

u8 SPI_WriteByte(u8 Byte)
{
    // Uart_Printf("Wait TX buffer empty\n\r");
	while(Macro_Check_Bit_Clear(SPI2->SR, 1));    // Wait until Tx buffer empty
	SPI2->DR = Byte;        // Byte 전송
    // Uart_Printf("Wait RX buffer not empty\n\r");
	while(Macro_Check_Bit_Clear(SPI2->SR, 0));      // Wait untill Rx buffer not empty
	return SPI2->DR;        // 받은 data는 리턴			
} 

void LCD_WriteRAM_Prepare(void)
{
	LCD_WR_REG(lcddev.wramcmd);
}	 

void Lcd_WriteData_16Bit(u16 Data)
{	
    LCD_CS_CLR;
    LCD_RS_SET;  
    SPI_WriteByte(Data>>8);
    SPI_WriteByte(Data);
    LCD_CS_SET;
}

void LCD_Clear(u16 Color)
{
    unsigned int i,m;  
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
    LCD_CS_CLR;
    LCD_RS_SET;
    for(i=0;i<lcddev.height;i++)
    {
    for(m=0;m<lcddev.width;m++)
    {	
        Lcd_WriteData_16Bit(Color);
    }
    }
    LCD_CS_SET;
} 

void LCD_Init(void)
{
	Uart_Printf("SPI Init step start\n\r");
    SPI2_Init();
	Uart_Printf("LCD GPIO Init step start\n\r");
    LCD_GPIO_Init();
	Uart_Printf("BLU PWM Init step start\n\r");
    BLU_PWM_Init();
	Uart_Printf("LCD RESET step start\n\r");
    LCD_Reset();

	Uart_Printf("LCD test\n\r");
	Uart_Printf("LCD_WR_REG() start\n\r");
	LCD_WR_REG(0xCF);  
	Uart_Printf("LCD_WR_DATA() start\n\r");
	LCD_WR_DATA(0x00); 
	Uart_Printf("LCD_WR_DATA() start\n\r");
	LCD_WR_DATA(0xD9); //0xC1 
	LCD_WR_DATA(0X30); 
	LCD_WR_REG(0xED);  
	LCD_WR_DATA(0x64); 
	LCD_WR_DATA(0x03); 
	LCD_WR_DATA(0X12); 
	LCD_WR_DATA(0X81); 
	LCD_WR_REG(0xE8);  
	LCD_WR_DATA(0x85); 
	LCD_WR_DATA(0x10); 
	LCD_WR_DATA(0x7A); 
	LCD_WR_REG(0xCB);  
	LCD_WR_DATA(0x39); 
	LCD_WR_DATA(0x2C); 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x34); 
	LCD_WR_DATA(0x02); 
	LCD_WR_REG(0xF7);  
	LCD_WR_DATA(0x20); 
	LCD_WR_REG(0xEA);  
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0xC0);    //Power control 
	LCD_WR_DATA(0x1B);   //VRH[5:0] 
	LCD_WR_REG(0xC1);    //Power control 
	LCD_WR_DATA(0x12);   //SAP[2:0];BT[3:0] 0x01
	LCD_WR_REG(0xC5);    //VCM control 
	LCD_WR_DATA(0x08); 	 //30
	LCD_WR_DATA(0x26); 	 //30
	LCD_WR_REG(0xC7);    //VCM control2 
	LCD_WR_DATA(0XB7); 
	LCD_WR_REG(0x36);    // Memory Access Control 
	LCD_WR_DATA(0x08); 
	LCD_WR_REG(0x3A);   
	LCD_WR_DATA(0x55); 
	LCD_WR_REG(0xB1);   
	LCD_WR_DATA(0x00);   
	LCD_WR_DATA(0x1A); 
	LCD_WR_REG(0xB6);    // Display Function Control 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0xA2); 
	LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
	LCD_WR_DATA(0x00); 
	LCD_WR_REG(0x26);    //Gamma curve selected 
	LCD_WR_DATA(0x01); 
	LCD_WR_REG(0xE0);    //Set Gamma 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x1D); 
	LCD_WR_DATA(0x1A); 
	LCD_WR_DATA(0x0A); 
	LCD_WR_DATA(0x0D); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x49); 
	LCD_WR_DATA(0X66); 
	LCD_WR_DATA(0x3B); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x11); 
	LCD_WR_DATA(0x01); 
	LCD_WR_DATA(0x09); 
	LCD_WR_DATA(0x05); 
	LCD_WR_DATA(0x04); 		 
	LCD_WR_REG(0XE1);    //Set Gamma 
	LCD_WR_DATA(0x00); 
	LCD_WR_DATA(0x18); 
	LCD_WR_DATA(0x1D); 
	LCD_WR_DATA(0x02); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x04); 
	LCD_WR_DATA(0x36); 
	LCD_WR_DATA(0x13); 
	LCD_WR_DATA(0x4C); 
	LCD_WR_DATA(0x07); 
	LCD_WR_DATA(0x13); 
	LCD_WR_DATA(0x0F); 
	LCD_WR_DATA(0x2E); 
	LCD_WR_DATA(0x2F); 
	LCD_WR_DATA(0x05); 
	LCD_WR_REG(0x2B); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A); 
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);	 
	LCD_WR_REG(0x11); //Exit Sleep
	SysTick_Delay_ms(120);
	LCD_WR_REG(0x29); //display on

	Uart_Printf("LCD_direction() start\n\r");
    LCD_direction(USE_HORIZONTAL); //LCD 표시 방향 설정
    int i;
    volatile int j;
    for (i = 0; i < 10; i++) //test
    {
        TIM1->CCR4 = TIM1->ARR * i / 10;
        for (j = 0; j < 100000; j++);
    }
	TIM1->CCR1 = 0;     // backlight 켜기 (duty 100%)
	// Macro_Set_Bit(GPIOB->ODR, 8);     // backlight 켜기 (duty 100%)
	LCD_Clear(BLUE);   // 전체화면 흰색
}

void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
{	
	LCD_WR_REG(lcddev.setxcmd);	
	LCD_WR_DATA(xStar>>8);
	LCD_WR_DATA(0x00FF&xStar);		
	LCD_WR_DATA(xEnd>>8);
	LCD_WR_DATA(0x00FF&xEnd);

	LCD_WR_REG(lcddev.setycmd);	
	LCD_WR_DATA(yStar>>8);
	LCD_WR_DATA(0x00FF&yStar);		
	LCD_WR_DATA(yEnd>>8);
	LCD_WR_DATA(0x00FF&yEnd);

	LCD_WriteRAM_Prepare();
}

void LCD_direction(u8 direction)
{ 
			lcddev.setxcmd=0x2A;
			lcddev.setycmd=0x2B;
			lcddev.wramcmd=0x2C;
	switch(direction){		  
		case 0:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;		
			LCD_WriteReg(0x36,(1<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 1:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;
		case 2:						 	 		
			lcddev.width=LCD_W;
			lcddev.height=LCD_H;	
			LCD_WriteReg(0x36,(1<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
		break;
		case 3:
			lcddev.width=LCD_H;
			lcddev.height=LCD_W;
			LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
		break;	
		default:break;
	}		
}	 