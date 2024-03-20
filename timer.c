#include "device_driver.h"

int TIM2_Check_Timeout(void)
{
	// 타이머가 timeout 이면 1 리턴, 아니면 0 리턴
	// timeout flag UIF 를 읽고나서는 0으로 만들어야함
	if (Macro_Check_Bit_Set(TIM2->SR, 0))
	{
		Macro_Clear_Bit(TIM2->SR, 0);
		return 1;
	}
	else return 0;

}
