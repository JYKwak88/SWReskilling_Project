#include "stm32f10x.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"

extern volatile int Uart1_Rx_In;
extern volatile char Uart1_Rx_Data;
extern volatile int Uart3_Rx_In;
extern volatile char Uart3_Rx_Data;
extern volatile int DRIVE_STATUS;
extern volatile int DIRECTION;
extern volatile int SPEED;
extern volatile int LIGHT_LEVEL;
extern volatile int NIGHT;
extern volatile int AUTO_LIGHT;
extern volatile int EMERGENCY;
extern volatile int BLINK_CNT;
extern volatile int LIGHT_ON;
extern volatile int NO_INPUT_CNT;
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
void SysTick_Delay_ms(unsigned int msec);

// Timer.c

// SysTick.c
extern void SysTick_Delay_ms(unsigned int msec);

// motor.c
#define TIM2_TICK         	(5) 				// usec
#define TIM2_FREQ 	  		(1000000/TIM2_TICK)	// Hz
#define TIM2_PLS_OF_10ms  	(10000/TIM2_TICK)
#define TIM2_UE_PERIOD      (2000)              // usec : PWM주기
#define TIM2_MAX	  		(0xffffu)

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

#define TIM4_TICK         	(50) 			// usec
#define TIM4_FREQ 	  		(1000000/TIM4_TICK)	// Hz
#define TIM4_PLS_OF_10ms  	(10000/TIM4_TICK)
#define TIM4_UE_PERIOD      (2000)              // usec : PWM주기
#define TIM4_MAX	  		(0xffffu)

extern void BlinkLED_Init(void);
extern void BlinkLED_Control(void);

extern void LED_Control(void);


// drivecar.c
extern void Start_Message(void);
extern void Print_State(void);

extern void Forward_Car(void);
extern void Back_Car(void);
extern void Stop_Car(void);
extern void Turn_Car(char input);

extern void Drive_Car(char input);

// cds.c
extern void CDS_Init(void);
extern void CDS_Start(void);
extern void CDS_Stop(void);
