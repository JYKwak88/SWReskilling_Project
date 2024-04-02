#include "device_driver.h"

void Wait_Bluetooth_Connect(void)
{
    LCD_Clear(BLACK);
    u16 t;
    POINT_COLOR = SKYBLUE;
    for (t = 10; t <= 20; t++)
    {
        LCD_DrawRectangle(t,t,LCD_H-t,LCD_W-t);
    }

    LCD_ShowString(t+3, t+1,      16, (u8*)"Wait for Bluetooth Connecting...", 1);
    LCD_ShowString(t+3, t+1+18*2, 16, (u8*)"Bluetooth NAME : HC-06", 1);
    LCD_ShowString(t+3, t+1+18*3, 16, (u8*)"Bluetooth PIN  : 1234", 1);
    LCD_ShowString(t+3, t+1+18*5, 16, (u8*)"1. Connect bluetooth", 1);
    LCD_ShowString(t+3, t+1+18*6, 16, (u8*)"2. then open terminal", 1);
    LCD_ShowString(t+3, t+1+18*7, 16, (u8*)"3. and send [S] key", 1);

    POINT_COLOR = WHITE;
    LCD_ShowString(t+3+8*8,  t+1+18*9,  16, (u8*)"LGE SW Reskilling Program", 1);
    LCD_ShowString(t+3+8*12, t+1+18*10, 16, (u8*)"jaeyoung.kwak@lge.com", 1);



    while (Uart_Rx_Data != 's')
	{
		Uart_Printf("Press [s] key if you see this message...\r");
		SysTick_Delay_ms(1000);
	}
	Uart_Rx_In = 0;
	Uart_Printf("\n\rControl via bluetooth COM port\n\r");
}

void Help_Message_Uart(void)
{
    Uart_Printf("= Move Control =\n\r");
    Uart_Printf("0:stop          |  1~5:select fwd speed   |\n\r");
    Uart_Printf("                |  w:go forward, speed++  |\n\r");
    Uart_Printf("a:turn left     |  s:stop                 |  d:turn right\n\r");
    Uart_Printf("                |  x:go back, speed--     |\n\r");
    Uart_Printf("= Light Control =\n\r");
    Uart_Printf("l:toggle light    |  o:toggle auto light  |  y:emergency light\n\r");
    Uart_Printf("p:auto brightness |   [,]:brightness -/+\n\r");
    Uart_Printf("\n\r");
    Print_State_Uart();
}

void Print_State_Uart(void)
{
    Uart_Printf("DRIVE_STATUS = %2d, DIRECTION = %2d, SPEED = %2d\n\r", DRIVE_STATUS, DIRECTION, SPEED);
}

void Forward_Car(void)
{
    if (SPEED <= 0) SPEED = 1;
    if (SPEED <= 2)
    {
        Motor_Drive(0, 5);
        SysTick_Delay_ms(50);
    }
    Motor_Drive(DIRECTION, SPEED);
    DRIVE_STATUS = 1;
    SEE_CENTER;
}

void Backward_Car(void)
{
    Motor_Drive(0, -2);
    SysTick_Delay_ms(50);
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

void Turn_Car(u8 input)
{
    if (input == 'a')
    {
        Uart_Printf("TURN LEFT\n\r");
        DIRECTION = -1;
        SEE_LEFT;
    }
    else if (input == 'd')
    {
        Uart_Printf("TURN RIGHT\n\r");
        DIRECTION = 1;
        SEE_RIGHT;
    }

    if (SPEED == 0)
    {
        Motor_Drive(DIRECTION, -2);
        SysTick_Delay_ms(50);
    }
    Motor_Drive(DIRECTION, SPEED);
}

void Drive_Car(u8 input)
{
    switch(input)
    {
    case 'a': case 'd':
        Turn_Car(input); return;
    case 's': case '0':
        DRIVE_STATUS = 0; break;
    case '1': case '2': case '3': case '4': case '5':
        SPEED = input - '0'; break;
    case 'w':
        if (DRIVE_STATUS == 1)
        {
            if (SPEED < 5) SPEED++;
        }
        else if (DRIVE_STATUS == 0) DRIVE_STATUS = 1;
        else if (DRIVE_STATUS == -1) DRIVE_STATUS = 0;
        break;
    case 'x':
        if (DRIVE_STATUS == 1)
        {
            if (SPEED > 1) SPEED--;
            else if (SPEED == 1)
            {
                SPEED = 0;
                DRIVE_STATUS = 0;
            }
        }
        else if (DRIVE_STATUS == 0) DRIVE_STATUS = -1;
        break;
    }

    if (DRIVE_STATUS == 1 && FRONT_DISTANCE > FRONT_LIMIT) Forward_Car();
    else if (DRIVE_STATUS == -1) Backward_Car();
    else Stop_Car();
    Draw_SpeedGage();
}