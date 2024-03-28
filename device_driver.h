#include "stm32f10x.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"
#include "stdlib.h"
#include "string.h"
#include "lcddefine.h"

// stm32f10x_it.c
// USART1
extern volatile u8 Uart1_Rx_In;
extern volatile u8 Uart1_Rx_Data;
// USART3
extern volatile u8 Uart3_Rx_In;
extern volatile u8 Uart3_Rx_Data;
// ADC1_2
extern volatile u32 EXT_LIGHT_LEVEL;
extern volatile u8 NIGHT;
// TIM4
extern volatile u8 EMERGENCY;
extern volatile u16 BLINK_CNT;
extern volatile s16 NO_INPUT_CNT;
extern volatile u8 CDS_WAIT_CNT;

// main.c
extern volatile enum _status DRIVE_STATUS;
extern volatile enum _direction DIRECTION;
extern volatile enum _speed SPEED;
extern volatile u8 LIGHT_ON;
extern volatile u8 AUTO_LIGHT;
extern volatile u8 LCD_AUTO_BRIGHTNESS;
extern volatile u32 LCD_BR_LEVEL;

// clock.c
void Clock_Init(void);

// Uart.c
// 개발중 Uart1, 실제구동 Uart3
#if DEV
#define Uart_Init			Uart1_Init
#define Uart_Send_Byte		Uart1_Send_Byte
#define Uart_Send_String	Uart1_Send_String
#define Uart_Printf			Uart1_Printf
#define Uart_Rx_In          Uart1_Rx_In
#define Uart_Rx_Data        Uart1_Rx_Data
#else
#define Uart_Init			Uart3_Init
#define Uart_Send_Byte		Uart3_Send_Byte
#define Uart_Send_String	Uart3_Send_String
#define Uart_Printf			Uart3_Printf
#define Uart_Rx_In          Uart3_Rx_In
#define Uart_Rx_Data        Uart3_Rx_Data
#endif

void Uart1_Init(int baud);
void Uart1_Send_Byte(char data);
void Uart1_Send_String(char *pt);
void Uart1_Printf(char *fmt,...);
char Uart1_Get_Char(void);
char Uart1_Get_Pressed(void);
void Uart3_Init(int baud);
void Uart3_Send_Byte(char data);
void Uart3_Send_String(char *pt);
void Uart3_Printf(char *fmt,...);
char Uart3_Get_Char(void);
char Uart3_Get_Pressed(void);

// SysTick.c
void SysTick_Delay_ms(u32 msec);

// Timer.c

// SysTick.c
void SysTick_Delay_ms(u32 msec);

// motor.c
void TIM2_Repeat(void);
void Motor_Init(void);
void Motor_Drive(s8 dir, s8 spd);

// extled.c
void H_R_LED_Init(void);
void HeadLED_On(void);
void HeadLED_Off(void);
void ReverseLED_On(void);
void ReverseLED_Off(void);
void TailLED_Init(void);
void TailLED_On(void);
void TailLED_Release(void);

void BlinkLED_Init(void);
void BlinkLED_Control(void);

void LED_Control(void);


// drivecar.c
void Wait_Bluetooth_Connect(void);
void Help_Message_Uart(void);
void Print_State_Uart(void);

void Forward_Car(void);
void Backward_Car(void);
void Stop_Car(void);
void Turn_Car(u8 input);

void Drive_Car(u8 input);

// cds.c
void CDS_Init(void);
void CDS_Start(void);
void CDS_Stop(void);

// lcd.c

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

extern u16 POINT_COLOR;
extern u16 BACK_COLOR;
extern u16 DeviceCode;

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
void LCD_DrawPoint(u16 x,u16 y);
void LCD_Clear(u16 Color);
void LCD_Init(void);
void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd);
void LCD_SetCursor(u16 Xpos, u16 Ypos);
void LCD_direction(u8 direction);

void Help_Message_LCD(void);
void LCD_LED_Toggle_Info(void);

// gui.c
void GUI_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);    //start x,y, end x,y, color
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);   //start x,y, end x,y
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void _draw_circle_8(int xc, int yc, int x, int y, u16 c);
void gui_circle(int xc, int yc,u16 c,int r, int fill);   // center x,y, color, radius, (1)fill/(0)unfill
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);   // 삼각형 꼭지점 3개 좌표
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode);	// start x,y, font color, background color, ascii-code, font size, (0)overlying/(1)non-overlying
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode);	// start x,y, font size, string pointer, (0)overlying/(1)non-overlying
void LCD_ShowMonobmp(u16 x,u16 y,u16 fc, u16 bc, u8 x_size, u8 y_size, u8 mode);
void GUI_DrawSpeedmeter(u16 x, u16 y, u16 fc, u16 bc, u8 mode);
void GUI_DrawSpeedmeter_BIG(u16 x, u16 y, u16 fc, u16 bc, u8 mode);
void Draw_LeftArrow(void);
void Draw_RightArrow(void);

extern const unsigned char Img_Speedmeter[1056];
