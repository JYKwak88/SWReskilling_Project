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
volatile int AUTO_LIGHT = 1;

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
	CDS_Init();
	Uart3_Init(9600);
	LED_Control();

#if (!DEV)
	while (Uart_Rx_Data != 's')
	{
		Uart_Printf("Press [s] key if you see this messege...\r");
		volatile int i;
		for (i = 0; i < 1000000; i++);
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
			else NO_INPUT_CNT = 600000/TIM4_UE_PERIOD;
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
			case 'o':
				AUTO_LIGHT ^= 1;
				if (AUTO_LIGHT == 1) CDS_Start();
				else if (AUTO_LIGHT == 0) CDS_Stop();
				LED_Control();
				// LCD 밝기 제어 추가 예정
				break;
			case 'l':
				LIGHT_ON ^= 1; Uart_Printf("LIGHT_ON = %d\n\r", LIGHT_ON); LED_Control(); break;
				// LCD 밝기 제어 추가 예정
			case 'y': // test
				EMERGENCY ^= 1; Uart_Printf("EMERGENCY = %d\n\r", EMERGENCY); BlinkLED_Control(); break;
			default :
				Uart_Printf("[%c] is Wrong Input\n\r", input);
			}
			pre_input = input;
		}

		LED_Control();

	}

}
