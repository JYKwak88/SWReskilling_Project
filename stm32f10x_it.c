/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "device_driver.h"

void Invalid_ISR(void)
{
  Uart1_Printf("Invalid_Exception: %d!\n", Macro_Extract_Area(SCB->ICSR, 0x1ff, 0));
  Uart1_Printf("Invalid_ISR: %d!\n", Macro_Extract_Area(SCB->ICSR, 0x1ff, 0) - 16);
  for(;;);
}

static char * const Stack_reg[] = {"R0","R1","R2","R3","R12","LR","RA","xPSR"};

static void Stack_Dump(const char * stack, unsigned int * sp)
{
	int i;

	for(i=0; i<(sizeof(Stack_reg)/sizeof(Stack_reg[0])); i++)
	{
		Uart1_Printf("%s[%d],%s=0x%.8X\n", stack, i, Stack_reg[i], sp[i]);
	}
}

static void Fault_Report(unsigned int * msp, unsigned int lr, unsigned int * psp)
{
	Uart1_Printf("LR(EXC_RETURN)=0x%.8X\n", lr);
	Uart1_Printf("MSP=0x%.8X\n", msp);
	Uart1_Printf("PSP=0x%.8X\n", psp);

	switch((lr & (0xF<<28))|(lr & 0xF))
	{
		case 0xF0000001: Uart1_Printf("Exception occurs from handler mode\n"); Stack_Dump("MSP", msp); break;
		case 0xF0000009: Uart1_Printf("Exception occurs from thread mode with MSP\n"); Stack_Dump("MSP", msp); break;
		case 0xF000000d: Uart1_Printf("Exception occurs from thread mode with PSP\n"); Stack_Dump("PSP", psp); break;
		default: Uart1_Printf("Invalid exception return value => %#.8X\n", lr & 0xf); break;
	}

	Uart1_Printf("SHCSR => %#.8X\n", SCB->SHCSR);
	Uart1_Printf("CFSR(Fault Reason) => %#.8X\n", SCB->CFSR);
}

/*******************************************************************************
 * Function Name  : NMIException
 * Description    : This function handles NMI exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void NMI_Handler(void)
{
	Uart1_Printf("NMI!\n");
	for(;;);
}

/*******************************************************************************
 * Function Name  : HardFaultException
 * Description    : This function handles Hard Fault exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void HardFault_Handler(unsigned int * msp, unsigned int lr, unsigned int * psp)
{
	Uart1_Printf("Hard Fault!\n");

	Fault_Report(msp, lr, psp);

	Uart1_Printf("MMFAR Valid => %d\n", Macro_Check_Bit_Set(SCB->CFSR, 7));
	Uart1_Printf("MMFAR => %#.8X\n", SCB->MMFAR);
	Uart1_Printf("BFAR Valid => %d\n", Macro_Check_Bit_Set(SCB->CFSR, 15));
	Uart1_Printf("BFAR => %#.8X\n", SCB->BFAR);
	Uart1_Printf("HFSR(Hard Fault Reason) => %#.8X\n", SCB->HFSR);

	for(;;);
}

/*******************************************************************************
 * Function Name  : MemManageException
 * Description    : This function handles Memory Manage exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void MemManage_Handler(unsigned int * msp, unsigned int lr, unsigned int * psp)
{
	Uart1_Printf("Memory Management Fault!\n");
	for(;;);
}

/*******************************************************************************
 * Function Name  : BusFaultException
 * Description    : This function handles Bus Fault exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void BusFault_Handler(unsigned int * msp, unsigned int lr, unsigned int * psp)
{
	Uart1_Printf("Bus Fault!\n");
	for(;;);
}

/*******************************************************************************
 * Function Name  : UsageFaultException
 * Description    : This function handles Usage Fault exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void UsageFault_Handler(unsigned int * msp, unsigned int lr, unsigned int * psp)
{
	Uart1_Printf("Usage Fault!\n");
	for(;;);
}

/*******************************************************************************
 * Function Name  : SVCHandler
 * Description    : This function handles SVCall exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SVC_Handler(void)
{
	Uart1_Printf("SVC Call\n");
	for(;;);
}

/*******************************************************************************
 * Function Name  : DebugMonitor
 * Description    : This function handles Debug Monitor exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DebugMon_Handler(void)
{
	Uart1_Printf("DebugMon Call\n");
	for(;;);
}

/*******************************************************************************
 * Function Name  : PendSVC
 * Description    : This function handles PendSVC exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void PendSV_Handler(void)
{
	Uart1_Printf("PendSV Call\n");
	for(;;);
}

/*******************************************************************************
 * Function Name  : SysTickHandler
 * Description    : This function handles SysTick Handler.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int SysTick_Flag = 0;

void SysTick_Handler(void)
{
	SysTick_Flag = 1;
}

/*******************************************************************************
 * Function Name  : WWDG_IRQHandler
 * Description    : This function handles WWDG interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void WWDG_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : PVD_IRQHandler
 * Description    : This function handles PVD interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void PVD_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TAMPER_IRQHandler
 * Description    : This function handles Tamper interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TAMPER_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : RTC_IRQHandler
 * Description    : This function handles RTC global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RTC_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : FLASH_IRQHandler
 * Description    : This function handles Flash interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void FLASH_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : RCC_IRQHandler
 * Description    : This function handles RCC interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RCC_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI0_IRQHandler
 * Description    : This function handles External interrupt Line 0 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI0_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI1_IRQHandler
 * Description    : This function handles External interrupt Line 1 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI2_IRQHandler
 * Description    : This function handles External interrupt Line 2 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI3_IRQHandler
 * Description    : This function handles External interrupt Line 3 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI3_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI4_IRQHandler
 * Description    : This function handles External interrupt Line 4 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI4_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel1_IRQHandler
 * Description    : This function handles DMA1 Channel 1 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel2_IRQHandler
 * Description    : This function handles DMA1 Channel 2 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel3_IRQHandler
 * Description    : This function handles DMA1 Channel 3 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel4_IRQHandler
 * Description    : This function handles DMA1 Channel 4 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel5_IRQHandler
 * Description    : This function handles DMA1 Channel 5 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel6_IRQHandler
 * Description    : This function handles DMA1 Channel 6 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel6_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel7_IRQHandler
 * Description    : This function handles DMA1 Channel 7 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : ADC1_2_IRQHandler
 * Description    : This function handles ADC1 and ADC2 global interrupts requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile u32 ILLUMINANCE = 0;
u8 NIGHT = 0;
u8 NIGHT_CHANGED = 0;
void ADC1_2_IRQHandler(void)
{
  ILLUMINANCE = ADC1->DR;
  if (NIGHT == 1 && ILLUMINANCE > 0xb00)
  {
    NIGHT = 0;
    NIGHT_CHANGED = 1;
  }
  else if (NIGHT == 0 && ILLUMINANCE < 0x900)
  {
    NIGHT = 1;
    NIGHT_CHANGED = 1;
  }

  Macro_Clear_Bit(ADC1->SR, 1);
  NVIC_ClearPendingIRQ(ADC1_2_IRQn);
}

/*******************************************************************************
 * Function Name  : USB_HP_CAN_TX_IRQHandler
 * Description    : This function handles USB High Priority or CAN TX interrupts
 *                  requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : USB_LP_CAN_RX0_IRQHandler
 * Description    : This function handles USB Low Priority or CAN RX0 interrupts
 *                  requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : CAN_RX1_IRQHandler
 * Description    : This function handles CAN RX1 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : CAN_SCE_IRQHandler
 * Description    : This function handles CAN SCE interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI9_5_IRQHandler
 * Description    : This function handles External lines 9 to 5 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int Key_Value = 0;

void EXTI9_5_IRQHandler(void)
{
	Key_Value = Macro_Extract_Area(EXTI->PR, 0x3, 6);

	EXTI->PR = 0x3<<6;
	NVIC_ClearPendingIRQ(23);
}

/*******************************************************************************
 * Function Name  : TIM1_BRK_IRQHandler
 * Description    : This function handles TIM1 Break interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM1_UP_IRQHandler
 * Description    : This function handles TIM1 overflow and update interrupt
 *                  request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
  Macro_Clear_Bit(TIM1->SR, 0);
  NVIC_ClearPendingIRQ(TIM1_UP_IRQn);
}

/*******************************************************************************
 * Function Name  : TIM1_TRG_COM_IRQHandler
 * Description    : This function handles TIM1 Trigger and commutation interrupts
 *                  requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM1_CC_IRQHandler
 * Description    : This function handles TIM1 capture compare interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_CC_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM2_IRQHandler
 * Description    : This function handles TIM2 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM2_IRQHandler(void)
{
  Macro_Clear_Bit(TIM2->SR, 0);
	NVIC_ClearPendingIRQ(TIM2_IRQn);
}

/*******************************************************************************
 * Function Name  : TIM3_IRQHandler
 * Description    : This function handles TIM3 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile u16 FRONT_START_CCR = 0;
volatile u16 FRONT_END_CCR = 0;
volatile u8 FRONT_CAPTURED = 0;
volatile u16 REAR_START_CCR = 0;
volatile u16 REAR_END_CCR = 0;
volatile u8 REAR_CAPTURED = 0;
void TIM3_IRQHandler(void)
{
  if (Macro_Check_Bit_Set(TIM3->SR, 1))   // TIM3_CH1 Interrupt
  {
    if (Macro_Check_Bit_Clear(TIM3->CCER, 1))   // rising edge capture 상태였으면
    {
      FRONT_START_CCR = TIM3->CCR1;   // 거리측정 시작의 CNT
      Macro_Set_Bit(TIM3->CCER, 1);   // falling edge capture 로 변경
    }
    else    // falling edge capture 상태였으면
    {
      FRONT_END_CCR = TIM3->CCR1;   // 거리측정 끝의 CNT
      FRONT_CAPTURED = 1;   // 측정완료를 알림
      Macro_Clear_Bit(TIM3->CCER, 1);   // rising edge capture 로 변경

      // Front capture 중단, Rear capture 시작
      Macro_Clear_Bit(TIM3->DIER, 1);
      Macro_Set_Bit(TIM3->DIER, 2);
    }
	  Macro_Clear_Bit(TIM3->SR, 1);   // TIM3_CH1 Interrupt 클리어
    NVIC_ClearPendingIRQ(TIM3_IRQn);
  }

  if (Macro_Check_Bit_Set(TIM3->SR, 2))   // TIM3_CH2 Interrupt
  {
    if (Macro_Check_Bit_Clear(TIM3->CCER, 5))   // rising edge capture 상태였으면
    {
      REAR_START_CCR = TIM3->CCR2;   // 거리측정 시작의 CNT
      Macro_Set_Bit(TIM3->CCER, 5);   // falling edge capture 로 변경
    }
    else    // falling edge capture 상태였으면
    {
      REAR_END_CCR = TIM3->CCR2;   // 거리측정 끝의 CNT
      REAR_CAPTURED = 1;   // 측정완료를 알림
      Macro_Clear_Bit(TIM3->CCER, 5);   // rising edge capture 로 변경

      // Rear capture 중단, Front capture 시작
      Macro_Clear_Bit(TIM3->DIER, 2);
      Macro_Set_Bit(TIM3->DIER, 1);
    }
	  Macro_Clear_Bit(TIM3->SR, 2);   // TIM3_CH2 Interrupt 클리어
    NVIC_ClearPendingIRQ(TIM3_IRQn);
  }


}

/*******************************************************************************
 * Function Name  : TIM4_IRQHandler
 * Description    : This function handles TIM4 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
u16 BLINK_CNT = 0;
u8 BLINK_CHANGED = 0;
s16 NO_INPUT_CNT = -1;
u8 CDS_WAIT_CNT = 0;
u8 USONIC_TRIG_CNT = 0;
void TIM4_IRQHandler(void)
{
  BLINK_CNT++;
  if (BLINK_CNT >= 500000/TIM4_UE_PERIOD)    // 500ms 마다 깜빡이
  {
    BLINK_CHANGED = 1;
    BLINK_CNT = 0;
  }

  if (NO_INPUT_CNT >= 300000/TIM4_UE_PERIOD)   // 300ms동안 입력이 없으면 가상입력으로 0을 보냄
  {
    Uart_Rx_In = 1;
    Uart_Rx_Data = 0;
  }
  else if (NO_INPUT_CNT > 0) NO_INPUT_CNT++;

  if (USONIC_TRIG_CNT >= 100000/TIM4_UE_PERIOD) // 100ms 마다 초음파센서 트리거
  {
    USONIC_TRIG_CNT = 1;
    TIM4->CCR4 = TIM4->ARR - 1;
  }
  else
  {
    TIM4->CCR4 = TIM4->ARR;
    USONIC_TRIG_CNT++;
  }

  CDS_WAIT_CNT++;
  if (CDS_WAIT_CNT > 20000/TIM4_UE_PERIOD)  // ADC 트리거를 자주 할 수록, BLU 변화가 부드러워짐 (20ms+20ms)
  {
    Macro_Set_Bit(ADC1->CR2, 22); 					// CDS Start (SW Trigger, EXTTRIG==1 일때만 됨)
    CDS_WAIT_CNT = 0;
  }

	Macro_Clear_Bit(TIM4->SR, 0);
	NVIC_ClearPendingIRQ(TIM4_IRQn);
}

/*******************************************************************************
 * Function Name  : I2C1_EV_IRQHandler
 * Description    : This function handles I2C1 Event interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : I2C1_ER_IRQHandler
 * Description    : This function handles I2C1 Error interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : I2C2_EV_IRQHandler
 * Description    : This function handles I2C2 Event interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : I2C2_ER_IRQHandler
 * Description    : This function handles I2C2 Error interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : SPI1_IRQHandler
 * Description    : This function handles SPI1 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SPI1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : SPI2_IRQHandler
 * Description    : This function handles SPI2 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SPI2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : USART1_IRQHandler
 * Description    : This function handles USART1 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile u8 Uart1_Rx_In = 0;
volatile u8 Uart1_Rx_Data = 0;

void USART1_IRQHandler(void)
{
	Uart1_Rx_Data = (unsigned char)USART1->DR;
	Uart1_Rx_In = 1;
	NVIC_ClearPendingIRQ(USART1_IRQn);
}

/*******************************************************************************
 * Function Name  : USART2_IRQHandler
 * Description    : This function handles USART2 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USART2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : USART3_IRQHandler
 * Description    : This function handles USART3 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile u8 Uart3_Rx_In = 0;
volatile u8 Uart3_Rx_Data = 0;
void USART3_IRQHandler(void)
{
	Uart3_Rx_Data = (unsigned char)USART3->DR;
	Uart3_Rx_In = 1;
	NVIC_ClearPendingIRQ(USART3_IRQn);
}

/*******************************************************************************
 * Function Name  : EXTI15_10_IRQHandler
 * Description    : This function handles External lines 15 to 10 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : RTCAlarm_IRQHandler
 * Description    : This function handles RTC Alarm interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : USBWakeUp_IRQHandler
 * Description    : This function handles USB WakeUp interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
  Invalid_ISR();
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
