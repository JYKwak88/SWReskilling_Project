#include "device_driver.h"

#define LIMIT(a) {if (a < 0 || a > 3000) a = 3000;}
u32 CENTER_DISTANCE;
u32 LEFT_DISTANCE;
u32 RIGHT_DISTANCE;


void Self_Driving(void)
{
    if (DRIVE_STATUS == 0)
    {
        SEE_CENTER;
        SysTick_Delay_ms(500);
        FRONT_CAPTURED = 0;
        while (!FRONT_CAPTURED);
        CENTER_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 1.7;  // 거리 계산
        LIMIT(CENTER_DISTANCE);
        Uart_Printf("CENTER=%d\n\r", CENTER_DISTANCE);
        if (CENTER_DISTANCE > FRONT_LIMIT*2)
        {
            Uart_Rx_Data = 'w';
            Uart_Rx_In = 1;
            return;
        }

        SEE_LEFT;
        SysTick_Delay_ms(500);
        FRONT_CAPTURED = 0;
        while (!FRONT_CAPTURED);
        LEFT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 1.7;  // 거리 계산
        LIMIT(LEFT_DISTANCE);
        Uart_Printf("LEFT=%d\n\r", LEFT_DISTANCE);
        SEE_RIGHT;
        SysTick_Delay_ms(500);
        FRONT_CAPTURED = 0;
        while (!FRONT_CAPTURED);
        RIGHT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 1.7;  // 거리 계산
        LIMIT(RIGHT_DISTANCE);
        Uart_Printf("RIGHT=%d\n\r", RIGHT_DISTANCE);
        if (LEFT_DISTANCE < FRONT_LIMIT && RIGHT_DISTANCE < FRONT_LIMIT)
        {
            Uart_Rx_Data = 'x';
            Uart_Rx_In = 1;
            return;
        }
        else if (LEFT_DISTANCE >= RIGHT_DISTANCE)
        {
            Uart_Rx_Data = 'a';
            Uart_Rx_In = 1;
            return;
        }
        else if (LEFT_DISTANCE < RIGHT_DISTANCE)
        {
            Uart_Rx_Data = 'd';
            Uart_Rx_In = 1;
            return;
        }
    }

    if (DRIVE_STATUS == 1)
    {
        SEE_CENTER;
        SysTick_Delay_ms(500);
        FRONT_CAPTURED = 0;
        while (!FRONT_CAPTURED);
        CENTER_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 1.7;  // 거리 계산
        LIMIT(CENTER_DISTANCE);
        Uart_Printf("CENTER=%d\n\r", CENTER_DISTANCE);
        if (CENTER_DISTANCE > FRONT_LIMIT * 3)
        {
            SEE_LEFT;
            SysTick_Delay_ms(500);
            FRONT_CAPTURED = 0;
            while (!FRONT_CAPTURED);
            LEFT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 1.7;  // 거리 계산
            LIMIT(LEFT_DISTANCE);
            Uart_Printf("LEFT=%d\n\r", LEFT_DISTANCE);
            SEE_RIGHT;
            SysTick_Delay_ms(500);
            FRONT_CAPTURED = 0;
            while (!FRONT_CAPTURED);
            RIGHT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 1.7;  // 거리 계산
            LIMIT(RIGHT_DISTANCE);
            Uart_Printf("RIGHT=%d\n\r", RIGHT_DISTANCE);
            if (LEFT_DISTANCE < FRONT_LIMIT || RIGHT_DISTANCE < FRONT_LIMIT)
            {
                Uart_Rx_Data = 's';
                Uart_Rx_In = 1;
                return;
            }
            else if (LEFT_DISTANCE < FRONT_LIMIT*2 || RIGHT_DISTANCE < FRONT_LIMIT*2)
            {
                if (LEFT_DISTANCE >= RIGHT_DISTANCE)
                {
                    if (SPEED > 2) SPEED = 2;
                    Uart_Rx_Data = 'a';
                    Uart_Rx_In = 1;
                    return;
                }
                else if (LEFT_DISTANCE < RIGHT_DISTANCE)
                {
                    if (SPEED > 2) SPEED = 2;
                    Uart_Rx_Data = 'd';
                    Uart_Rx_In = 1;
                    return;
                }
            }
            else
            {
                Uart_Rx_Data = 'w';
                Uart_Rx_In = 1;
                return;
            }
        }
    }

    if (DRIVE_STATUS == -1)
    {
        SEE_CENTER;
        SysTick_Delay_ms(500);
        FRONT_CAPTURED = 0;
        while (!FRONT_CAPTURED);
        CENTER_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 1.7;  // 거리 계산
        LIMIT(CENTER_DISTANCE);
        Uart_Printf("CENTER=%d\n\r", CENTER_DISTANCE);
        if (CENTER_DISTANCE > FRONT_LIMIT * 2)
        {
            DRIVE_STATUS = 0;
            Uart_Rx_Data = 'a';
            Uart_Rx_In = 1;
            return;
        }

    }
}