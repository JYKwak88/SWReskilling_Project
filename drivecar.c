#include "device_driver.h"

void Start_Message(void)
{
    Uart_Printf("========== Move Control ==========\n\r");
    Uart_Printf("[0]:stop       |  [1]~[5]:select fwd speed  |\n\r");
    Uart_Printf("               |  [w]:go forward, speed++   |\n\r");
    Uart_Printf("[a]:turn left  |  [s]:stop                  |  [d]:turn right\n\r");
    Uart_Printf("               |  [x]:go back               |\n\r");
    Uart_Printf("========== Light Control =========\n\r");
    Uart_Printf("[l]:toggle light  |  [o]:toggle auto light  |  [y]:emergency light\n\r");
    Uart_Printf("\n\r");
    Print_State();
}

void Print_State(void)
{
    Uart_Printf("DRIVE_STATUS = %2d, DIRECTION = %2d, SPEED = %2d\n\r", DRIVE_STATUS, DIRECTION, SPEED);
}

void Forward_Car(void)
{
    if (SPEED <= 0) SPEED = 1;
    if (SPEED <= 2)
    {
        Motor_Drive(0, 5);
        SysTick_Delay_ms(100);
    }
    Motor_Drive(DIRECTION, SPEED);
    DRIVE_STATUS = 1;
}

void Back_Car(void)
{
    Motor_Drive(0, -2);
    SysTick_Delay_ms(100);
    SPEED = -1;
    Motor_Drive(DIRECTION, SPEED);
    DRIVE_STATUS = -1;
}

void Stop_Car(void)
{
    SPEED = 0;
    Motor_Drive(DIRECTION, SPEED);
    DRIVE_STATUS = 0;
}

void Turn_Car(char input)
{
    if (input == 'a')
    {
        Uart_Printf("TURN LEFT\n\r");
        DIRECTION = -1;
    }
    else if (input == 'd')
    {
        Uart_Printf("TURN RIGHT\n\r");
        DIRECTION = 1;
    }
    // if (DRIVE_STATUS == 0) return;
    Motor_Drive(DIRECTION, SPEED);
}

void Drive_Car(char input)
{
    switch(input)
    {
    case 'a': case 'd':
        Turn_Car(input);
        return;
    case 's': case '0':
        DRIVE_STATUS = 0; break;
    case '1': case '2': case '3': case '4': case '5':
        SPEED = (int)(input - '0'); break;
    case 'w':
        if (DRIVE_STATUS == 1)
        {
            if (SPEED < 5) SPEED++;
        }
        else if (DRIVE_STATUS == 0) DRIVE_STATUS = 1;
        else if (DRIVE_STATUS == -1) DRIVE_STATUS = 0;
        break;
    case 'x':
        if (DRIVE_STATUS == 1) DRIVE_STATUS = 0;
        else if (DRIVE_STATUS == 0) DRIVE_STATUS = -1;
        break;
    }
    
    if (DRIVE_STATUS == 1) Forward_Car();
    else if (DRIVE_STATUS == -1) Back_Car();
    else if (DRIVE_STATUS == 0) Stop_Car();
}