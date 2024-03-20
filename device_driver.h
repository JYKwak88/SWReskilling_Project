#include "stm32f10x.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"

extern volatile int Uart1_Rx_In;
extern volatile char Uart1_Rx_Data;
extern volatile int Uart3_Rx_In;
extern volatile char Uart3_Rx_Data;
extern volatile int STATUS;
extern volatile int DIRECTION;
extern volatile int SPEED;
extern volatile int NIGHT;
extern volatile int EMERGENCY;
extern volatile int BLINK_CNT;
extern volatile int LIGHT_ON;
extern volatile int NO_INPUT_CNT;
// Uart.c
// 개발중 Uart = 1, 실제구동 Uart = 3
#define DEV (0)
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

extern void Uart1_Init(int baud);
extern void Uart1_Send_Byte(char data);
extern void Uart1_Send_String(char *pt);
extern void Uart1_Printf(char *fmt,...);
extern char Uart1_Get_Char(void);
extern char Uart1_Get_Pressed(void);
extern void Uart3_Init(int baud);
extern void Uart3_Send_Byte(char data);
extern void Uart3_Send_String(char *pt);
extern void Uart3_Printf(char *fmt,...);
extern char Uart3_Get_Char(void);
extern char Uart3_Get_Pressed(void);


// Led.c
extern void LED_Init(void);
extern void LED_Display(unsigned int num);
extern void LED_All_On(void);
extern void LED_All_Off(void);

// Clock.c
extern void Clock_Init(void);

// Key.c
extern void Key_Poll_Init(void);
extern int Key_Get_Pressed(void);
extern void Key_Wait_Key_Released(void);
extern int Key_Wait_Key_Pressed(void);
extern void Key_ISR_Enable(int en);

// SysTick.c
extern void SysTick_Run(unsigned int msec);
extern int SysTick_Check_Timeout(void);
extern unsigned int SysTick_Get_Time(void);
extern unsigned int SysTick_Get_Load_Time(void);
extern void SysTick_Stop(void);
extern void SysTick_OS_Tick(unsigned int msec);

// Timer.c
extern int TIM2_Check_Timeout(void);
extern void TIM4_Repeat(void);
extern int TIM4_Check_Timeout(void);


// SysTick.c

// motor.c
extern void TIM2_Repeat(void);
extern void Motor_Init(void);
extern void Motor_Drive(int dir, int spd);

// extled.c
extern void H_R_LED_Init(void);
extern void HeadLED_On(void);
extern void HeadLED_Off(void);
extern void ReverseLED_On(void);
extern void ReverseLED_Off(void);
extern void TailLED_Init(void);
extern void TailLED_On(void);
extern void TailLED_Release(void);
extern void BlinkLED_Init(void);
extern void BlinkLED_Control(void);

extern void LED_Control(int spd);


// drivecar.c
extern void Print_State(void);

extern void Forward_Car(void);
extern void Back_Car(void);
extern void Stop_Car(void);
extern void Turn_Car(char input);