#include "device_driver.h"

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
        CENTER_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 17;  // 거리 계산
        if (CENTER_DISTANCE > FRONT_LIMIT)
        {
            Drive_Car('w');
            EMERGENCY = 0;
            Draw_Emergency();
            return;
        }

        for (;;)
        {
            SEE_LEFT;
            SysTick_Delay_ms(500);
            FRONT_CAPTURED = 0;
            while (!FRONT_CAPTURED);
            LEFT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 17;  // 거리 계산

            SEE_RIGHT;
            SysTick_Delay_ms(500);
            FRONT_CAPTURED = 0;
            while (!FRONT_CAPTURED);
            RIGHT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 17;  // 거리 계산

            if (LEFT_DISTANCE < FRONT_LIMIT && RIGHT_DISTANCE < FRONT_LIMIT)
            {
                Drive_Car('x');
                SysTick_Delay_ms(2000);
                Drive_Car('s');
            }
            else if (LEFT_DISTANCE >= RIGHT_DISTANCE)
            {
                Drive_Car('a');
                SysTick_Delay_ms(500);
                SPEED = 2;
                Drive_Car('w');
                EMERGENCY = 0;
                Draw_Emergency();
                return;
            }
            else if (LEFT_DISTANCE < RIGHT_DISTANCE)
            {
                Drive_Car('d');
                SysTick_Delay_ms(500);
                SPEED = 2;
                Drive_Car('w');
                EMERGENCY = 0;
                Draw_Emergency();
                return;
            }
        }
    }

    if (DRIVE_STATUS == 1)
    {
        SEE_CENTER;
        SysTick_Delay_ms(500);
        FRONT_CAPTURED = 0;
        while (!FRONT_CAPTURED);
        CENTER_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 17;  // 거리 계산
        if (CENTER_DISTANCE > FRONT_LIMIT * 5)
        {
            SEE_LEFT;
            SysTick_Delay_ms(500);
            FRONT_CAPTURED = 0;
            while (!FRONT_CAPTURED);
            LEFT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 17;  // 거리 계산

            SEE_RIGHT;
            SysTick_Delay_ms(500);
            FRONT_CAPTURED = 0;
            while (!FRONT_CAPTURED);
            RIGHT_DISTANCE = (FRONT_START_CCR - FRONT_END_CCR) * 17;  // 거리 계산

            if (LEFT_DISTANCE < FRONT_LIMIT*2 || RIGHT_DISTANCE < FRONT_LIMIT*2)
            {
                if (LEFT_DISTANCE >= RIGHT_DISTANCE)
                {
                    if (SPEED > 1) SPEED--;
                    Drive_Car('a');
                    SysTick_Delay_ms(500);
				    DIRECTION = center;
                    Drive_Car(0);
                    return;
                }
                else if (LEFT_DISTANCE < RIGHT_DISTANCE)
                {
                    if (SPEED > 1) SPEED--;
                    Drive_Car('d');
                    SysTick_Delay_ms(500);
				    DIRECTION = center;
                    Drive_Car(0);
                    return;
                }
            }
            else
            {
                Drive_Car('w');
                return;
            }
        }
    }
}