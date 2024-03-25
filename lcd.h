#include "device_driver.h"
#include "stdlib.h"


//LCD 중요한 매개변수 세트
typedef struct  
{										    
	u16 width;			//LCD width
	u16 height;			//LCD height
	u16 id;				  //LCD ID
	u8  dir;			  //화면 방향 : 0, 세로; 1, 가로	
	u16	 wramcmd;		//gram명령 작성 시작??
	u16  setxcmd;		//x 좌표 설정 명령
	u16  setycmd;		//y 좌표 설정 명령	 
}_lcd_dev; 	

extern _lcd_dev lcddev;

#define USE_HORIZONTAL  (1)   //LCD 화면의 시계 방향 회전 방향 정의: 0-0도 회전, 1-90도 회전, 2-180도 회전, 3-270도 회전
#define LCD_W (240)
#define LCD_H (320)

//브러시 색상
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F  
#define BRED        0XF81F
#define GRED 			 	0XFFE0
#define GBLUE			 	0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 			0XBC40 //갈색
#define BRRED 			0XFC07 //적갈색
#define GRAY  			0X8430
//GUI 색상

#define DARKBLUE      	 0X01CF
#define LIGHTBLUE      	 0X7D7C
#define GRAYBLUE       	 0X5458
//위 3가지 색상은 PANEL 색상입니다. 
 
#define LIGHTGREEN     	0X841F
#define LIGHTGRAY     0XEF5B // (PANNEL)
#define LGRAY 			 		0XC618 //밝은회색(PANNEL),형태배경색

#define LGRAYBLUE      	0XA651 //(중간레이어컬러)
#define LBBLUE          0X2B12 //연한갈색파란색(항목의반대색상, 선택)


//함수
void SPI2_Init(void);
void LCD_GPIO_Init(void);
void BLU_PWM_Init(void);

#define LCD_RST_SET     GPIOA->BSRR=(1<<6)
#define LCD_RST_CLR     GPIOA->BRR=(1<<6)
#define LCD_CS_SET      GPIOB->BSRR=(1<<12)
#define LCD_CS_CLR      GPIOB->BRR=(1<<12)
#define LCD_RS_SET      GPIOA->BSRR=(1<<7)
#define LCD_RS_CLR      GPIOA->BRR=(1<<7)

void SPI2_Init(void);
void LCD_GPIO_Init(void);
void BLU_PWM_Init(void);
void LCD_Reset(void);
void LCD_WR_REG(u8 data);
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue);
void LCD_WR_DATA(u8 data);
u8 SPI_WriteByte(u8 Byte);
void LCD_WriteRAM_Prepare(void);
void Lcd_WriteData_16Bit(u16 Data);
void LCD_Clear(u16 Color);
void LCD_Init(void);
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_direction(u8 direction);