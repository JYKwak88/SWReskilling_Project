#include "device_driver.h"

static void Sys_Init(void)
{
	Clock_Init();

	Uart1_Init(115200);

	SCB->VTOR = 0x08003000;
	SCB->SHCSR = 0;
}

volatile int SPEED;
volatile int STATUS;
volatile int LIGHT_ON = 0;

void Main(void)
{
	enum _direction{left=-1, center=0, right=1};
	enum _gear{back2=-2, back=-1, stop=0, one, two, three, four, five};
	enum _status{reverse=-1, idle=0, forward=1};
	Sys_Init();
	char input = 0;
	char pre_input = '0';

	STATUS = idle;
	DIRECTION = center;
	SPEED = stop;
	
	Motor_Init();
	H_R_LED_Init();
	TailLED_Init();
	LED_Control(SPEED);

#if !(DEV)
	Uart1_Printf("Press [s] key after connect to bluetooth COM Port\n\r");
	while (Uart1_Rx_Data != 's');
	Uart1_Rx_In = 0;
	Uart3_Init(9600);
	Uart1_Printf("Control via bluetooth COM port\n\r");
#endif

	// int interlock0 = 1;
	// int interlock1 = 1;

	for(;;)
	{
		/* 키 입력 받으면 방향, 속도 계속 유지 하는 방식*/
		#if 0
		if (Uart_Rx_In)
		{
			input = Uart_Rx_Data;
			Uart_Rx_In = 0;
			switch (input)
			{
			case '0': case 'x':
				SPEED = stop; break;
			case '1': case '2': case '3': case '4': case '5':
				SPEED = (int)(input - '0'); break;
			case 'w':
				if (SPEED < 5) SPEED++; break;
			case 's':
				if (SPEED > -2) SPEED--; break;
			case 'd':
				if (DIRECTION < 1) DIRECTION++; break;
			case 'a':
				if (DIRECTION > -1) DIRECTION--; break;
			case 'l': // test
				LIGHT_ON ^= 1; Uart_Printf("LIGHT_ON = %d\n\r", LIGHT_ON); break;
			case 'y': // test
				EMERGENCY ^= 1; Uart_Printf("EMERGENCY = %d\n\r", EMERGENCY); break;
			case 'n': // test
				NIGHT ^= 1; Uart_Printf("NIGHT = %d\n\r", NIGHT); break;
			default :
				Uart_Printf("Wrong Input\n\r");
			}
			Uart_Printf("DIRECTION = %d, SPEED = %d\n\r", DIRECTION, SPEED);
			Motor_Drive(DIRECTION, SPEED);
			LED_Control(SPEED);
			BlinkLED_Control();

		}
		#endif

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
			else NO_INPUT_CNT = 300;
			if (input != pre_input) switch (input)
			{
			case 0:
				NO_INPUT_CNT = -1;
				DIRECTION = 0;
				if (STATUS == 1) Forward_Car();
				else if (STATUS == -1) Back_Car();
				else if (STATUS == 0) Stop_Car();
				BlinkLED_Control();
				break;
			case '0': case 's':
				Stop_Car(); break;
			case '1': case '2': case '3': case '4': case '5':
				SPEED = (int)(input - '0');
				if (STATUS == 1) Forward_Car();
				else if (STATUS == -1) Stop_Car();
				break;
			case 'w':
				if (STATUS == 0) Forward_Car();
				else if (STATUS == -1) Stop_Car();
				break;
			case 'x':
				if (STATUS == 1) Stop_Car();
				else if (STATUS == 0) Back_Car();
				break;
			case 'a':
				DIRECTION = -1;
				Turn_Car(input);
				break;
			case 'd':
				DIRECTION = 1;
				Turn_Car(input);
				break;
			case 'l': // test
				LIGHT_ON ^= 1; Uart_Printf("LIGHT_ON = %d\n\r", LIGHT_ON); LED_Control(SPEED); break;
			case 'y': // test
				EMERGENCY ^= 1; Uart_Printf("EMERGENCY = %d\n\r", EMERGENCY); BlinkLED_Control(); break;
			case 'n': // test
				NIGHT ^= 1; Uart_Printf("NIGHT = %d\n\r", NIGHT); LED_Control(SPEED); break;
			default :
				Uart_Printf("[%c] is Wrong Input\n\r", input);
			}
			pre_input = input;
		}


#if 0
		// key로 속도 조절 - test 용
		Key_Poll_Init();
		
		if ((interlock0 != 0) && Macro_Check_Bit_Clear(GPIOB->IDR, 6))
		{
			if (SPEED < 5) SPEED++;
			interlock0 = 0;
		}
		else if ((interlock0 == 0) && Macro_Check_Bit_Set(GPIOB->IDR, 6))
		{
			interlock0 = 1;
		}

		if ((interlock1 != 0) && Macro_Check_Bit_Clear(GPIOB->IDR, 7))
		{
			if (SPEED > 0) SPEED--;
			interlock1 = 0;
		}
		else if ((interlock1 == 0) && Macro_Check_Bit_Set(GPIOB->IDR, 7))
		{
			interlock1 = 1;
		}
#endif
	}

}
