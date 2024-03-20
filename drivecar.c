#include "device_driver.h"

void Print_State(void)
{
    Uart_Printf("STATUS = %2d, DIRECTION = %2d, SPEED = %2d\n\r", STATUS, DIRECTION, SPEED);
}

void Forward_Car(void)
{
    if (SPEED <= 0) SPEED = 1;
    if (SPEED <= 2)
    {
        volatile int delay;
        Motor_Drive(0, 5);
        for (delay = 0; delay < 50000; delay++);
    }
    Motor_Drive(DIRECTION, SPEED);
	LED_Control(SPEED);
    STATUS = 1;
    Print_State();
}

void Back_Car(void)
{
    Motor_Drive(0, -2);
    volatile int delay;
    for (delay = 0; delay < 50000; delay++);
    SPEED = -1;
    Motor_Drive(DIRECTION, SPEED);
    LED_Control(SPEED);
    STATUS = -1;
    Print_State();
}

void Stop_Car(void)
{
    SPEED = 0;
    Motor_Drive(DIRECTION, SPEED);
    LED_Control(SPEED);
    STATUS = 0;
    Print_State();
}

void Turn_Car(char input)
{
    if (input == 'a') Uart_Printf("TURN LEFT\n\r");
    else if (input == 'd') Uart_Printf("TURN RIGHT\n\r");
	BlinkLED_Control();
    Print_State();
    if (STATUS == 0) return;
    Motor_Drive(DIRECTION, SPEED);
}