#include "device_driver.h"
#include "lcd.h"
/****************************************************************************************************
// SCK          SPI2_SCK(PB13)
// SDI(MOSI)    SPI2_MOSI(PB15)
// SDO(MISO)    SPI2_MISO(PB14)
// CS           SPI2_NSS(PB12)
// Back Light   TIM3_CH1(PB6)
// DC/RS        PA7
// RST          PA6
**************************************************************************************************/	
#define SPI2_BR_OP (0)
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
// DC/RS        PA7
// RST          PA6
	Macro_Set_Area(RCC->APB2ENR, 0x3, 2);       // PA PB Enable
    Macro_Write_Block(GPIOA->CRL, 0xf, 0x3, 24);            // PA6 GPO P-P 50MHz
    Macro_Write_Block(GPIOA->CRL, 0xf, 0x3, 28);            // PA7 GPO P-P 50MHz
    // Macro_Write_Block(GPIOB->CRL, 0xf, 0x3, 20);            // PB5 GPO P-P 50MHz
}

void BLU_PWM_Init(void)
{
    //extled.c 의 TailLED_Init()에서 TIM3 init
    Macro_Write_Block(GPIOB->CRL, 0xf, 0xa, 24);
    // 초기 출력(high)은 duty 100%
    TIM4->CCR1 = TIM4->ARR;
    // OUT pin : active high, output enable
    Macro_Write_Block(TIM4->CCMR1, 0xff, 0x68, 0);
	Macro_Write_Block(TIM4->CCER, 0x3, 0x1, 0);
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
    // SysTick_Delay_ms(20);
    SPI_WriteByte(data);
    // SysTick_Delay_ms(20);
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
    SPI2_Init();
    LCD_GPIO_Init();
    BLU_PWM_Init();

    LCD_Reset();

	LCD_WR_REG(0xCF);   //Power control B
	LCD_WR_DATA(0x00);  //1st: 00
	LCD_WR_DATA(0xD9);  //0xC1? 2nd: D9 : DDVDH=VCI*2, VGH=VCI*6, VGL=-VCI*3
	LCD_WR_DATA(0X30);  //3rd: 30 : Discharge path enable
	LCD_WR_REG(0xED);   //Power on sequence control
	LCD_WR_DATA(0x64);  //1st: 64 : CP1 soft start keep 3 frame, CP23 soft start keep 3 frame
	LCD_WR_DATA(0x03);  //2nd: 03 : VCL 1st frame enable, DDVDH 4th frame enable
	LCD_WR_DATA(0X12);  //3rd: 12 : VGH 2nd frame enable, VGL 3rd frame enable
	LCD_WR_DATA(0X81);  //4th: 81 : DDVDH enhanced mode enable
	LCD_WR_REG(0xE8);   //Driver timing contral A
	LCD_WR_DATA(0x85);  //1st: 85 : gate driver non-overlap timing contorl : default + 1unit
	LCD_WR_DATA(0x10);  //2nd: 10 : EQ timing=default, CR timing=default - 1unit
	LCD_WR_DATA(0x7A);  //3rd: 7A : pre-charge timing contorl : default
	LCD_WR_REG(0xCB);   //Power control A
	LCD_WR_DATA(0x39);  //1st: 39 : fix value
	LCD_WR_DATA(0x2C);  //2nd: 2C : fix value
	LCD_WR_DATA(0x00);  //3rd: 00 : fix value
	LCD_WR_DATA(0x34);  //4th: 34 : REG_VD=1.6V
	LCD_WR_DATA(0x02);  //5th: 02 : DDVDH=5.6V
	LCD_WR_REG(0xF7);   //Pump ratio control
	LCD_WR_DATA(0x20);  //20 : ratio=DDVDH=2*VCI
	LCD_WR_REG(0xEA);   //Driver timing control B
	LCD_WR_DATA(0x00);  //1st: gate driver timing control 00 : EQ2GND,EQ2DDVDH,EQ2DDVDH,EQ2GND all 0unit
	LCD_WR_DATA(0x00);  //2nd: 00 : fix value
	LCD_WR_REG(0xC0);   //Power control 1
	LCD_WR_DATA(0x1B);  //1st: 1B : GVDD=4.20V
	LCD_WR_REG(0xC1);   //Power control 2
	LCD_WR_DATA(0x12);  //12 : DDVDH=VCI*2, VGH=VCI*6, VHL=-VCI*4
	LCD_WR_REG(0xC5);   //VCOM control 1
	LCD_WR_DATA(0x08); 	//1st: 08 : VCOMH=2.900V
	LCD_WR_DATA(0x26); 	//2nd: 26 : VCOML=-1.550V
	LCD_WR_REG(0xC7);   //VCOM control 2
	LCD_WR_DATA(0XB7);  //1st: B7 : nVM=1, VCOMH=VMH-9, VCOML=VML-9
	LCD_WR_REG(0x36);   //Memory Access Control
	LCD_WR_DATA(0x08);  //08 : MY=MX=MV=ML=0, BGR=1, MH=0 : 
	LCD_WR_REG(0x3A);   //COLMOD: Pixel Format Set
	LCD_WR_DATA(0x55);  //55 : RGB=16bits/pixel, MCU=16bits/pixel
	LCD_WR_REG(0xB1);   //Frame Rate Control
	LCD_WR_DATA(0x00);  //1st: 00 : Division ratio=f_osc (no divide)
	LCD_WR_DATA(0x1A);  //2nd: 1A : Frame Rate=73Hz
	LCD_WR_REG(0xB6);   //Display Function Control
	LCD_WR_DATA(0x0A);  //1st: 0A : the scan mode in non-display area : Interval scan... see 164page
	LCD_WR_DATA(0xA2);  //2nd: A2 : Liquid crystal type=Normally white, Scan Cycle=5frames ... see 165page
	LCD_WR_REG(0xF2);   //Enable 3Gamma Function
	LCD_WR_DATA(0x00);  //00 : disable
	LCD_WR_REG(0x26);   //Gamma Set
	LCD_WR_DATA(0x01);  //01 : Gamma curve 1
	LCD_WR_REG(0xE0);   //Positive Gamma Correction
	LCD_WR_DATA(0x0F);  //
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
	LCD_WR_REG(0XE1);   //Negative Gamma Correction
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
	LCD_WR_REG(0x2B);   //Page Address Set
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x01);
	LCD_WR_DATA(0x3f);
	LCD_WR_REG(0x2A);   //Column Address Set
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0x00);
	LCD_WR_DATA(0xef);
	LCD_WR_REG(0x11); //Sleep out (no param)
	SysTick_Delay_ms(120);
	LCD_WR_REG(0x29); //Display on (no param)

	// Uart_Printf("LCD_direction() start\n\r");
    LCD_direction(USE_HORIZONTAL); //LCD 표시 방향 설정
    
	LCD_Clear(BLUE);   // 전체화면 파랑
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