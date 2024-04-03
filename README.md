LGE SW Reskilling Program Project
주제 : Cortex-M3 (STM32F103) 으로 자동차 흉내내기
구성
 * 좌우 모터 제어 : PWM (TIM2_CH1~4)
  - PWM 주기 : 2ms(500Hz)
  - 모터드라이버 : L298N mini (MX1508)
  - DC모터 : 아두이노용 노랑모터
 * 전조등, 후진등 : GPIO (PA4, PA5)
 * 후미등(브레이크등) : PWM (TIM4_CH2)
 * 방향지시등 : GPIO (L:PA13, R:PA14), TIM4 20ms*25번 카운트하여 점멸
 * LCD 제어 : SPI2
  - LCD 모듈 : MSP2402 (ILI9341)
 * LCD Backlight 제어 : PWM (TIM4_CH1), ADC_IN9 (CDS센서)
 * 입력 제어 : UART1 or 3
  - 개발, 디버깅 : UART1 - PC
  - 실제동작 : UART3 - BLUETOOTH(HC-06) - PC
 * 전/후방 거리 감지 초음파 센서
  - TRIG : TIM4_CH3 20ms*5번=100ms 마다 TRIG
  - Front ECHO : TIM3_CH1 Rising/Falling 전환
  - Rear ECHO : TIM3_CH2 Rising/Falling 전환
  - ECHO간 간섭 방지를 위해 CH1, CH2 교대로 Interrupt

v1.0 (2024-04-03)