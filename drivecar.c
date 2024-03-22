#include "device_driver.h"

void Start_Message(void)
{
    Uart_Printf("========== Engine Start ==========\n\r");
    Uart_Printf("\n\r");
    Uart_Printf("========== Move Control ==========\n\r");
    Uart_Printf("[1]~[5]:select forward speed\n\r");
    Uart_Printf("               |  [w]:go forward  |\n\r");
    Uart_Printf("[a]:turn left  |  [s]:stop        |  [d]:turn right\n\r");
    Uart_Printf("               |  [x]:go back     |\n\r");
    Uart_Printf("========== Light Control =========\n\r");
    Uart_Printf("[l]:toggle light  |  [o]:toggle auto light  |  [y]:emergency light\n\r");
    Uart_Printf("\n\r");
    Print_State();
}

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
    STATUS = 1;
}

void Back_Car(void)
{
    Motor_Drive(0, -2);
    volatile int delay;
    for (delay = 0; delay < 50000; delay++);
    SPEED = -1;
    Motor_Drive(DIRECTION, SPEED);
    STATUS = -1;
}

void Stop_Car(void)
{
    SPEED = 0;
    Motor_Drive(DIRECTION, SPEED);
    STATUS = 0;
}

void Turn_Car(char input)
{
    if (input == 'a') Uart_Printf("TURN LEFT\n\r");
    else if (input == 'd') Uart_Printf("TURN RIGHT\n\r");
    if (STATUS == 0) return;
    Motor_Drive(DIRECTION, SPEED);
}