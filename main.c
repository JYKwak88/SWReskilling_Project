#include "device_driver.h"

static void Sys_Init(void)
{
	Clock_Init();

	Uart1_Init(115200);

	SCB->VTOR = 0x08003000;
	SCB->SHCSR = 0;
}

volatile enum _status DRIVE_STATUS = idle;
volatile enum _direction DIRECTION = center;
volatile enum _speed SPEED = stop;
volatile u8 LIGHT_ON = 0;
volatile u8 AUTO_LIGHT = 1;
volatile u8 LCD_AUTO_BRIGHTNESS = 1;
volatile u32 LCD_BR_LEVEL = 0xb00;

void Main(void)
{

	Sys_Init();
	u8 input = 0;
	u8 pre_input = '0';

	DRIVE_STATUS = idle;
	DIRECTION = center;
	SPEED = stop;

	Motor_Init();
	H_R_LED_Init();
	TailLED_Init();
	LED_Control();
	CDS_Init();
	Uart3_Init(9600);

	LCD_Init();

#if (!DEV)
	Wait_Bluetooth_Connect();
	LCD_Clear(BLACK);
#endif

	Help_Message_Uart();
	Help_Message_LCD();
	GUI_DrawSpeedmeter(METER_CENTER_X-METER_W/2, METER_CENTER_Y-METER_H/2, METER_COLOR, METER_BACK_COLOR, 1);
	// GUI_DrawSpeedmeter_BIG(30,30,METER_COLOR, METER_BACK_COLOR, 1);
	Draw_LeftArrow();
	Draw_RightArrow();


	for(;;)
	{
		/* 키 입력 눌리는 동안만 구동하는 방식을 하려고 했으나... */
		/* 터미널프로그램은 동시입력 지원이 안되고, 키 릴리즈 이벤트를 보낼 수 없다
		   때문에, 전진키, 후진키 한번으로 0번방향으로 출발하고, 중간에 단수 설정 가능하고
		   전진 중 후진 누르면 멈추고, 후진 중 전진 누르면 멈추고 0,x 키도 멈추고
		   왼쪽, 오른쪽만 방향 계속 유지하도록 해야함
		*/

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
				Drive_Car(0);
			}

			if (input != pre_input)
			{
				switch (input)
				{
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

				case 'l':
					LIGHT_ON ^= 1;
					Uart_Printf("LIGHT_ON = %d\n\r", LIGHT_ON);
					LCD_LED_Toggle_Info();
					break;
				case 'y':
					EMERGENCY ^= 1;
					Uart_Printf("EMERGENCY = %d\n\r", EMERGENCY);
					LCD_LED_Toggle_Info();
					break;
				case 'o':
					AUTO_LIGHT ^= 1;
					Uart_Printf("AUTO_LIGHT = %d\n\r", AUTO_LIGHT);
					LCD_LED_Toggle_Info();
					break;

				case 'p':
					LCD_AUTO_BRIGHTNESS ^= 1;
					Uart_Printf("LCD_AUTO_BRIGHTNESS = %d\n\r", LCD_AUTO_BRIGHTNESS);
					LCD_LED_Toggle_Info();
					break;
				case '[':
					LCD_AUTO_BRIGHTNESS = 0;
					if (LCD_BR_LEVEL > 0) LCD_BR_LEVEL--;
					Uart_Printf("LCD_AUTO_BRIGHTNESS DISABLE, LCD_BL_LEVEL = %d\n\r", LCD_BR_LEVEL);
					LCD_LED_Toggle_Info();
					break;
				case ']':
					LCD_AUTO_BRIGHTNESS = 0;
					if (LCD_BR_LEVEL < 100) LCD_BR_LEVEL++;
					Uart_Printf("LCD_AUTO_BRIGHTNESS DISABLE, LCD_BL_LEVEL = %d\n\r", LCD_BR_LEVEL);
					LCD_LED_Toggle_Info();
					break;

				case 'h':
					Help_Message_Uart();
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

		if (LCD_AUTO_BRIGHTNESS)
		{
			LCD_BR_LEVEL = EXT_LIGHT_LEVEL * 100 / 0xfff;
		}
		TIM4->CCR1 = TIM4->ARR * LCD_BR_LEVEL / 100;


	}

}
