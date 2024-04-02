#define DEV (1)     // 개발중 : 1, 실제구동 : 0

#define SYSCLK	72000000
#define HCLK	SYSCLK
#define PCLK2	HCLK
#define PCLK1	(HCLK/2)
#define TIMXCLK ((HCLK == PCLK1)?(PCLK1):(PCLK1*2))

#define RAM_START	0x20000000
#define RAM_END		0x20004FFF
#define HEAP_BASE	(((unsigned int)&__ZI_LIMIT__ + 0x7) & ~0x7)
#define HEAP_SIZE	(4*1024)
#define HEAP_LIMIT	(HEAP_BASE + HEAP_SIZE)
#define STACK_LIMIT	(HEAP_LIMIT + 8)
#define STACK_BASE	(RAM_END + 1)
#define STACK_SIZE	(STACK_BASE - STACK_LIMIT)

enum _status{reverse=-1, idle=0, forward=1};
enum _direction{left=-1, center=0, right=1};
enum _speed{back2=-2, back=-1, stop=0, one, two, three, four, five};

// motor.c
#define TIM2_TICK         	(5) 				// usec
#define TIM2_FREQ 	  		(1000000/TIM2_TICK)	// Hz
#define TIM2_PLS_OF_10ms  	(10000/TIM2_TICK)
#define TIM2_UE_PERIOD      (2000)              // usec : PWM주기
#define TIM2_MAX	  		(0xffffu)

// extled.c
#define TIM4_TICK         	(50) 			// usec
#define TIM4_FREQ 	  		(1000000/TIM4_TICK)	// Hz
#define TIM4_PLS_OF_10ms  	(10000/TIM4_TICK)
#define TIM4_UE_PERIOD      (20000)              // usec : PWM주기
#define TIM4_MAX	  		(0xffffu)

// ultrasonic.c
#define TIM3_TICK         	(10) 			// usec
#define TIM3_FREQ 	  		(1000000/TIM3_TICK)	// Hz
#define TIM3_PLS_OF_10ms  	(10000/TIM3_TICK)
#define TIM3_UE_PERIOD      (100000)              // usec
#define TIM3_MAX	  		(18000/TIM3_TICK)

#define FRONT_LIMIT (2000)
#define REAR_LIMIT  (1500)