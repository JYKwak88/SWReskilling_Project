#include "device_driver.h"

static void Sys_Init(void)
{
	Clock_Init();
	Uart1_Init(115200);

	SCB->VTOR = 0x08003000;
	SCB->SHCSR = 0;

	// Init 순서 유지할 것
	Motor_Init();
	H_R_LED_Init();
	TailLED_Init();
	CDS_Init();
	Uart3_Init(9600);

	Servo_Init();
	USONIC_TRIG_Init();
	USONIC_ECHO_Init();

	LCD_Init();
}

enum _status DRIVE_STATUS = idle;
enum _direction DIRECTION = center;
enum _speed SPEED = stop;
u8 LIGHT_ON = 0;
u8 EMERGENCY = 0;
u8 AUTO_LIGHT = 1;
u8 LCD_AUTO_BRIGHTNESS = 1;
u8 LCD_BL_LEVEL = 0;
u8 METER_Z = 2;
u32 FRONT_DISTANCE = 0;  // unit : mm
u32 REAR_DISTANCE = 0;  // unit : mm
u8 FRONT_DETECT = 0;
u8 REAR_DETECT = 0;

void Main(void)
{
	Sys_Init();
	u8 input = 0;
	u8 pre_input = '0';

	DRIVE_STATUS = idle;
	DIRECTION = center;
	SPEED = stop;

#if (!DEV)
	Wait_Bluetooth_Connect();
#endif

	Help_Message_Uart();
	Screen_Init();
	LED_Control();

	for(;;)
	{
		if (Uart_Rx_In)
		{
			input = Uart_Rx_Data;
			Uart_Rx_In = 0;
			if (input >= 'A' && input <= 'Z') input += -'A' + 'a';
			if (input == 'a' || input == 'd') NO_INPUT_CNT = 1;
			else
			{
				NO_INPUT_CNT = -1;
				DIRECTION = center;
				SEE_CENTER;
			}


			if (input != pre_input)
			{
				switch (input)
				{
				// 모터 구동에 관한 입력
				case 0:
					NO_INPUT_CNT = -1;
					DIRECTION = center;
				case 's': case '0':
				case '1': case '2': case '3': case '4': case '5':
				case 'w':
				case 'x':
				case 'a': case 'd':
					Drive_Car(input);
					break;

				// LED, LCD 상태 변경 입력
				case 'l': case 'y': case 'o': case 'p':
					if (input == 'l')
					{
						LIGHT_ON ^= 1;
						Uart_Printf("LIGHT(%d)\n\r", LIGHT_ON);
					}
					if (input == 'y')
					{
						EMERGENCY ^= 1;
						Draw_Emergency();
						Uart_Printf("EMERGENCY(%d)\n\r", EMERGENCY);
					}
					if (input == 'o')
					{
						AUTO_LIGHT ^= 1;
						Uart_Printf("AUTO_LIGHT(%d)\n\r", AUTO_LIGHT);
					}
					if (input == 'p')
					{
						LCD_AUTO_BRIGHTNESS ^= 1;
						Uart_Printf("LCD_AUTO_BRIGHTNESS(%d)\n\r", LCD_AUTO_BRIGHTNESS);
					}
					LCD_LED_Toggle_Info();
					break;

				// LCD 밝기 수동 조절 입력
				case '[': case ']':		// ascii 133, 135
					if (LCD_AUTO_BRIGHTNESS)
					{
						LCD_AUTO_BRIGHTNESS = 0;
						Uart_Printf("LCD_AUTO_BRIGHTNESS(%d)\n\r", LCD_AUTO_BRIGHTNESS);
					}
					if 		(input == '[' && LCD_BL_LEVEL > 0) 			 LCD_BL_LEVEL--;
					else if (input == ']' && LCD_BL_LEVEL < LCD_BL_STEP) LCD_BL_LEVEL++;
					Uart_Printf("LCD_BL_LEVEL = %d\n\r", LCD_BL_LEVEL * 100 / LCD_BL_STEP);
					Show_Brightness();
					LCD_LED_Toggle_Info();
					break;

				case 'h':
					METER_Z ^= 3;// 1 <-> 2
					Help_Message_Uart();
					Screen_Init();
					break;
				default:
					Uart_Printf("[%c] is Wrong Input\n\r", input);
    				Uart_Printf("Press 'H' key if you see the key guide\n\r");
				}

				Print_State_Uart();
				LED_Control();
			}
			if (input == 'a' || input == 'd') pre_input = input;
			else pre_input = 0;

		}

		if (NIGHT_CHANGED)
		{
    		LED_Control();
			NIGHT_CHANGED = 0;
		}

		if (LCD_AUTO_BRIGHTNESS)
		{
			TIM4->CCR1 = TIM4->ARR * ILLUMINANCE / 0xfff;	// ADC resolution = 0xfff
			LCD_BL_LEVEL = ILLUMINANCE * LCD_BL_STEP / 0xfff;
			Show_Brightness();
		}
		else if (input == '[' || input == ']')
		{
			TIM4->CCR1 = TIM4->ARR * (LCD_BL_LEVEL + 1) / (LCD_BL_STEP + 1);
		}

		if (BLINK_CHANGED)
		{
			if (EMERGENCY == 1) BOTH_LED_INVERT;
    		else if (DIRECTION == -1) L_LED_INVERT;
    		else if (DIRECTION == 1) R_LED_INVERT;
			Draw_Arrow();
			BLINK_CHANGED = 0;
		}

		if (FRONT_CAPTURED)
		{
			FRONT_CAPTURED = 0;
		    FRONT_DISTANCE = Calc_Dist_Front();  // 거리 계산
			// Uart_Printf("FRONTDIST = %4d mm, REARDIST = %4d mm\n\r", FRONT_DISTANCE, REAR_DISTANCE);

			if (FRONT_DETECT == 0 && FRONT_DISTANCE < FRONT_LIMIT)
			{
				FRONT_DETECT = 1;
				Uart_Printf("FRONT_DIST=%4dmm(%d)\n\r", FRONT_DISTANCE, FRONT_DETECT);
				Draw_Frontsensor();
				if (DRIVE_STATUS == 1)
				{
					Drive_Car('0');
					EMERGENCY = 1;
					Draw_Emergency();
					Print_State_Uart();
					LED_Control();
				}
			}
			else if (FRONT_DETECT == 1 && FRONT_DISTANCE > FRONT_LIMIT*1.2)
			{
				FRONT_DETECT = 0;
				Uart_Printf("FRONT_DIST=%4dmm(%d)\n\r", FRONT_DISTANCE, FRONT_DETECT);
				Draw_Frontsensor();
			}
		}
		if (REAR_CAPTURED)
		{
			REAR_CAPTURED = 0;
		    REAR_DISTANCE = Calc_Dist_Rear();  // 거리 계산
			// Uart_Printf("FRONTDIST = %4d mm, REARDIST = %4d mm\n\r", FRONT_DISTANCE, REAR_DISTANCE);

			if (REAR_DETECT == 0 && REAR_DISTANCE < REAR_LIMIT)
			{
				REAR_DETECT = 1;
				Uart_Printf("REAR_DIST=%4dmm(%d)\n\r", REAR_DISTANCE, REAR_DETECT);
				Draw_Rearsensor();
				if (DRIVE_STATUS == -1)
				{
					Drive_Car('0');
					EMERGENCY = 1;
					Draw_Emergency();
					Print_State_Uart();
					LED_Control();
				}
			}
			else if (REAR_DETECT == 1 && REAR_DISTANCE > REAR_LIMIT*1.2)
			{
				REAR_DETECT = 0;
				Uart_Printf("REAR_DIST=%4dmm(%d)\n\r", REAR_DISTANCE, REAR_DETECT);
				Draw_Rearsensor();
			}
		}
	}
}
