#define USE_HORIZONTAL  (1)   //LCD 화면의 시계 방향 회전 방향 정의: 0-0도 회전, 1-90도 회전, 2-180도 회전, 3-270도 회전
#define LCD_W (240)
#define LCD_H (320)

#define HELP_FONT_COLOR WHITE
#define MOVE_HELP_BACK_COLOR BLUE
#define LED_HELP_BACK_COLOR LBBLUE
#define METER_COLOR 0xa534
#define METER_BACK_COLOR BLACK
extern u8 METER_Z;
#define METER_W (96 * METER_Z)
#define METER_H (88 * METER_Z)
#define METER_CENTER_X (160)
#define METER_CENTER_Y (LCD_W - METER_H/2 - 10)
#define METER_PIN_LEN   (22 * METER_Z)
#define METER_PIN_OFFSET    (-5 * METER_Z)
#define R_X     (METER_CENTER_X - METER_PIN_LEN * 0.71)
#define R_Y     (METER_CENTER_Y + METER_PIN_LEN * 0.71)
#define ZERO_X  (METER_CENTER_X - METER_PIN_LEN)
#define ZERO_Y  (METER_CENTER_Y)
#define ONE_X   (METER_CENTER_X - METER_PIN_LEN * 0.71)
#define ONE_Y   (METER_CENTER_Y - METER_PIN_LEN * 0.71)
#define TWO_X   (METER_CENTER_X)
#define TWO_Y   (METER_CENTER_Y - METER_PIN_LEN)
#define THREE_X (METER_CENTER_X + METER_PIN_LEN * 0.71)
#define THREE_Y (METER_CENTER_Y - METER_PIN_LEN * 0.71)
#define FOUR_X  (METER_CENTER_X + METER_PIN_LEN)
#define FOUR_Y  (METER_CENTER_Y)
#define FIVE_X  (METER_CENTER_X + METER_PIN_LEN * 0.71)
#define FIVE_Y  (METER_CENTER_Y + METER_PIN_LEN * 0.71)
#define LEFT_X (METER_CENTER_X - ARROW_SIZE*2)
#define LEFT_Y (METER_CENTER_Y - METER_H / 2 - ARROW_SIZE - (8 * METER_Z))
#define RIGHT_X (METER_CENTER_X + ARROW_SIZE + METER_Z)
#define RIGHT_Y (METER_CENTER_Y - METER_H / 2 - ARROW_SIZE - (8 * METER_Z))
#define ARROW_SIZE (14 * METER_Z)
#define EMER_X (METER_CENTER_X - ARROW_SIZE / 2)
#define EMER_Y (METER_CENTER_Y - METER_H / 2 - ARROW_SIZE - (8 * METER_Z))

#define LIGHT_ON_COLOR LIGHTGREEN
#define LIGHT_OFF_COLOR DARKGRAY
#define LIGHT_ON_X (METER_CENTER_X + METER_W/METER_Z + 5*METER_Z)
#define LIGHT_ON_Y (METER_CENTER_Y + 10 * METER_Z)

#define TOPVIEW_W (24 * METER_Z)
#define TOPVIEW_H (48 * METER_Z)
#define TOPVIEW_CENTER_X (METER_CENTER_X - METER_W/METER_Z - 18*METER_Z)
#define TOPVIEW_CENTER_Y (METER_CENTER_Y + 15 * METER_Z)

#define TOPVIEW_X (TOPVIEW_CENTER_X - TOPVIEW_W / 2)
#define TOPVIEW_Y (TOPVIEW_CENTER_Y - TOPVIEW_H / 2)

#define WHITE       0xFFFF
#define BLACK      	0x0000
#define BLUE       	0x001F
#define VIOLET      0xF81F
#define SKYBLUE		0x07FF
#define RED         0xF800
#define GREEN       0x07E0
#define DARKGREEN   0x0500
#define YELLOW      0xFFE0
#define BROWN 		0xBC40
#define ORANGE 		0xFC07
#define GRAY  		0x8430
#define DARKGRAY	0x39C7
#define DARKBLUE    0x01CF
#define LIGHTBLUE   0x7D7C
#define GRAYBLUE    0x5458
#define LIGHTGREEN  0x67EC
#define LIGHTGRAY   0xEF5B // (PANNEL)
#define LGRAY 		0xC618 //밝은회색(PANNEL),형태배경색
#define LGRAYBLUE   0xA651 //(중간레이어컬러)
#define LBBLUE      0x2B12 //연한갈색파란색(항목의반대색상, 선택)

#define LCD_BL_STEP     (20)    // 100을 n(최대100)단계로 나누어 표현 = LCD_BL_LEVEL의 최대값
