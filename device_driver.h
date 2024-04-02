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
extern volatile u32 ILLUMINANCE;	//외부 조도
extern u8 NIGHT;			//1,야간; 0,주간
extern u8 NIGHT_CHANGED;
// TIM3
extern volatile u16 FRONT_START_CCR;
extern volatile u16 FRONT_END_CCR;
extern volatile u8 FRONT_CAPTURED;
extern volatile u16 REAR_START_CCR;
extern volatile u16 REAR_END_CCR;
extern volatile u8 REAR_CAPTURED;

// TIM4
extern u16 BLINK_CNT;		//깜빡이 toggle 시점 cnt
extern u8 BLINK_CHANGED;	//깜빡이 업데이트 필요
extern s16 NO_INPUT_CNT;	//UART 입력 없음 판단용 cnt
extern u8 CDS_WAIT_CNT;	//조도센서 측정 시점 cnt

// main.c
extern enum _status DRIVE_STATUS;
extern enum _direction DIRECTION;
extern enum _speed SPEED;
extern u8 LIGHT_ON;			//사용자 LED on/off
extern u8 EMERGENCY;			//비상등
extern u8 AUTO_LIGHT;			//외부조도에 따른 자동 LED on/off
extern u8 LCD_AUTO_BRIGHTNESS;	//외부조도에 따른 자동 LCD 밝기 조절 on/off
extern u8 LCD_BL_LEVEL;		//LCD 밝기 단계 (max = LCD_BL_STEP, defined)
extern u32 FRONT_DISTANCE;
extern u32 REAR_DISTANCE;

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

#define BLINK_PORT	GPIOA		// pin 바꿀 때 extled.c의 BlinkLED_Init 수정 필요!
#define L_LED_PIN	(13)
#define R_LED_PIN	(14)
#define L_LED_ON
#define R_LED_ON
#define BOTH_LED_ON
#define L_LED_OFF			(Macro_Set_Bit(BLINK_PORT->ODR, L_LED_PIN))
#define R_LED_OFF			(Macro_Set_Bit(BLINK_PORT->ODR, R_LED_PIN))
#define BOTH_LED_OFF		(Macro_Set_Area(BLINK_PORT->ODR, 0x3, L_LED_PIN))		// 안쓰는게 좋겠음
#define L_LED_INVERT		(Macro_Invert_Bit(BLINK_PORT->ODR, L_LED_PIN))
#define R_LED_INVERT		(Macro_Invert_Bit(BLINK_PORT->ODR, R_LED_PIN))
#define BOTH_LED_INVERT		(Macro_Invert_Area(BLINK_PORT->ODR, 0x3, L_LED_PIN))	// 안쓰는게 좋겠음
#define L_LED_CHECK_ON		(Macro_Check_Bit_Clear(BLINK_PORT->ODR, L_LED_PIN))
#define R_LED_CHECK_ON		(Macro_Check_Bit_Clear(BLINK_PORT->ODR, R_LED_PIN))


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

#define LCD_RST_SET     GPIOA->BSRR=(1<<11)
#define LCD_RST_CLR     GPIOA->BRR=(1<<11)
#define LCD_CS_SET      GPIOB->BSRR=(1<<12)
#define LCD_CS_CLR      GPIOB->BRR=(1<<12)
#define LCD_RS_SET      GPIOA->BSRR=(1<<15)
#define LCD_RS_CLR      GPIOA->BRR=(1<<15)

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

void Screen_Init(void);
void LCD_LED_Toggle_Info(void);
void Show_Brightness(void);

// gui.c
void GUI_DrawPoint(u16 x,u16 y,u16 color);
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);    //start x,y, end x,y, color
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);   //start x,y, end x,y
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2);
void _draw_circle_8(int xc, int yc, int x, int y, u16 c);
void GUI_Circle(int xc, int yc,u16 c,int r, int fill);   // center x,y, color, radius, (1)fill/(0)unfill
void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);   // 삼각형 꼭지점 3개 좌표
void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2);
void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode);	// start x,y, font color, background color, ascii-code, font size, (0)overlying/(1)non-overlying
void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode);	// start x,y, font size, string pointer, (0)overlying/(1)non-overlying
void Draw_monoBMP(u16 x, u16 y, u16 size_x, u16 size_y, u16 fc, const unsigned char * img, u8 grad);
void Draw_LeftArrow(void);
void Draw_RightArrow(void);
void Draw_Arrow(void);
void Draw_Emergency(void);
void Draw_SpeedGage(void);
void Draw_Frontsensor(void);
void Draw_Rearsensor(void);

extern const unsigned char Img_Speedmeter[1056];
extern const unsigned char Img_Lighton[54];
extern const unsigned char Img_Topview[144];
extern const unsigned char Img_Wall[18];

// ultrasonic.c
#define SEE_LEFT	(TIM4->CCR3 = TIM4->ARR * 9.8 / 100)
#define SEE_CENTER	(TIM4->CCR3 = TIM4->ARR * 6.7 / 100)
#define SEE_RIGHT	(TIM4->CCR3 = TIM4->ARR * 4 / 100)
void Servo_Init(void);
void USONIC_TRIG_Init(void);
void USONIC_ECHO_Init(void);
