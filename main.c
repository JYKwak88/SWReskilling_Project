#include "device_driver.h"
#include "lcd.h"
static void Sys_Init(void)
{
	Clock_Init();

	Uart1_Init(115200);

	SCB->VTOR = 0x08003000;
	SCB->SHCSR = 0;
}

volatile int SPEED;
volatile int DRIVE_STATUS;
volatile int LIGHT_ON = 0;
volatile int AUTO_LIGHT = 1;
volatile int LCD_AUTO_BRIGHTNESS = 1;
volatile int LCD_BR_LEVEL = 0xb00;

void Main(void)
{
	enum _direction{left=-1, center=0, right=1};
	enum _gear{back2=-2, back=-1, stop=0, one, two, three, four, five};
	enum _status{reverse=-1, idle=0, forward=1};
	Sys_Init();
	char input = 0;
	char pre_input = '0';

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
	while (Uart_Rx_Data != 's')
	{
		Uart_Printf("Press [s] key if you see this messege...\r");
		SysTick_Delay_ms(1000);
	}
	Uart_Rx_In = 0;
	Uart_Printf("\n\rControl via bluetooth COM port\n\r");
		
#endif
	Start_Message();

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
			if (input == 'a' || input == 'd') NO_INPUT_CNT = 1;
			else
			{
				NO_INPUT_CNT = -1;
				DIRECTION = 0;
				Drive_Car(0);
			}

			if (input != pre_input)
			{
				switch (input)
				{
				case 0:
					NO_INPUT_CNT = -1;
					DIRECTION = 0;
				case 's': case '0':
				case '1': case '2': case '3': case '4': case '5':
				case 'w':
				case 'x':
				case 'a': case 'd':
					Drive_Car(input);
					break;

				case 'o':
					AUTO_LIGHT ^= 1; Uart_Printf("AUTO_LIGHT = %d\n\r", AUTO_LIGHT); break;
				case 'l':
					LIGHT_ON ^= 1; Uart_Printf("LIGHT_ON = %d\n\r", LIGHT_ON); break;
				case 'y':
					EMERGENCY ^= 1; Uart_Printf("EMERGENCY = %d\n\r", EMERGENCY); break;
				
				case 'p':
					LCD_AUTO_BRIGHTNESS ^= 1; Uart_Printf("LCD_AUTO_BRIGHTNESS = %d\n\r", LCD_AUTO_BRIGHTNESS); break;
				case '[':
					LCD_AUTO_BRIGHTNESS = 0;
					if (LCD_BR_LEVEL > 0) LCD_BR_LEVEL--;
					Uart_Printf("LCD_AUTO_BRIGHTNESS DISABLE, LCD_BL_LEVEL = %d\n\r", LCD_BR_LEVEL);
					break;
				case ']':
					LCD_AUTO_BRIGHTNESS = 0;
					if (LCD_BR_LEVEL < 100) LCD_BR_LEVEL++;
					Uart_Printf("LCD_AUTO_BRIGHTNESS DISABLE, LCD_BL_LEVEL = %d\n\r", LCD_BR_LEVEL);
					break;

				default:
					Uart_Printf("[%c] is Wrong Input\n\r", input);
				}

				Print_State();
				LED_Control();
			}
			if (input == 'a' || input == 'd') pre_input = input;
			else pre_input = 0;
			
		}

		if (LCD_AUTO_BRIGHTNESS)
		{
			LCD_BR_LEVEL = LIGHT_LEVEL * 100 / 0xfff;
		}
		TIM4->CCR1 = TIM4->ARR * LCD_BR_LEVEL / 100;

	}

}
