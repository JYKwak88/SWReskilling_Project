#include "device_driver.h"
#include "font.h"

void GUI_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_SetCursor(x,y);
	Lcd_WriteData_16Bit(color);
}

void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)    //start x,y, end x,y, color
{
	u16 i,j;
	u16 width=ex-sx+1; 		//채우기 너비
	u16 height=ey-sy+1;		//채우기 높이
	LCD_SetWindows(sx,sy,ex,ey); //윈도우 설정
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		Lcd_WriteData_16Bit(color);	//데이터 기록
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1); //종료 후 전체창으로 복원
}

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)   //start x,y, end x,y
{
	u16 t;
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;

	delta_x=x2-x1;
	delta_y=y2-y1;
	uRow=x1;
	uCol=y1;

	if (delta_x>0) incx=1;
	else if (delta_x==0) incx=0;//수직선
	else {incx=-1; delta_x=-delta_x;}

	if (delta_y>0)incy=1;
	else if (delta_y==0) incy=0;//수평선
	else {incy=-1; delta_y=-delta_y;}

    if (delta_x>delta_y) distance=delta_x;
	else distance=delta_y;

    for(t=0;t<=distance+1;t++ )//선그리기
	{
		LCD_DrawPoint(uRow,uCol);
		xerr+=delta_x ;
		yerr+=delta_y ;
		if(xerr>distance)
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_DrawLine(x1,y1,x2,y1);
	LCD_DrawLine(x1,y1,x1,y2);
	LCD_DrawLine(x1,y2,x2,y2);
	LCD_DrawLine(x2,y1,x2,y2);
}

void LCD_DrawFillRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
	LCD_Fill(x1,y1,x2,y2,POINT_COLOR);
}

void _draw_circle_8(int xc, int yc, int x, int y, u16 c)
{
	GUI_DrawPoint(xc + x, yc + y, c);
    GUI_DrawPoint(xc - x, yc + y, c);
	GUI_DrawPoint(xc + x, yc - y, c);
	GUI_DrawPoint(xc - x, yc - y, c);
	GUI_DrawPoint(xc + y, yc + x, c);
	GUI_DrawPoint(xc - y, yc + x, c);
	GUI_DrawPoint(xc + y, yc - x, c);
	GUI_DrawPoint(xc - y, yc - x, c);
}

void GUI_Circle(int xc, int yc,u16 c,int r, int fill)   // center x,y, color, radius, (1)fill/(0)unfill
{
	int x = 0, y = r, yi, d;
	d = 3 - 2 * r;

	if (fill)
	{
		// solid circle
		while (x <= y) {
			for (yi = x; yi <= y; yi++)
				_draw_circle_8(xc, yc, x, yi, c);

			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	} else
	{
		// empty circle
		while (x <= y) {
			_draw_circle_8(xc, yc, x, y, c);
			if (d < 0) {
				d = d + 4 * x + 6;
			} else {
				d = d + 4 * (x - y) + 10;
				y--;
			}
			x++;
		}
	}
}

void Draw_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2)   // 삼각형 꼭지점 3개 좌표
{
	LCD_DrawLine(x0,y0,x1,y1);
	LCD_DrawLine(x1,y1,x2,y2);
	LCD_DrawLine(x2,y2,x0,y0);
}

static void _swap(u16 *a, u16 *b)
{
	u16 tmp;
  tmp = *a;
	*a = *b;
	*b = tmp;
}

void Fill_Triangel(u16 x0,u16 y0,u16 x1,u16 y1,u16 x2,u16 y2)
{
	u16 a, b, y, last;
	int dx01, dy01, dx02, dy02, dx12, dy12;
	long sa = 0;
	long sb = 0;
	if (y0 > y1)
	{
		_swap(&y0, &y1);
		_swap(&x0, &x1);
	}
	if (y1 > y2)
	{
		_swap(&y2, &y1);
		_swap(&x2, &x1);
	}
	if (y0 > y1)
	{
		_swap(&y0, &y1);
		_swap(&x0, &x1);
	}
	if (y0 == y2)
	{
		a = b = x0;
		if (x1 < a) a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a) a = x2;
		else if (x2 > b) b = x2;
		LCD_Fill(a, y0, b, y0, POINT_COLOR);
		return;
	}
	dx01 = x1 - x0;
	dy01 = y1 - y0;
	dx02 = x2 - x0;
	dy02 = y2 - y0;
	dx12 = x2 - x1;
	dy12 = y2 - y1;

	if (y1 == y2) last = y1;
	else last = y1 - 1;

	for (y = y0; y <= last; y++)
	{
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;
		if (a > b) _swap(&a, &b);
		LCD_Fill(a, y, b, y, POINT_COLOR);
	}
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++)
	{
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;
		if (a > b) _swap(&a, &b);
		LCD_Fill(a, y, b, y, POINT_COLOR);
	}
}

void LCD_ShowChar(u16 x,u16 y,u16 fc, u16 bc, u8 num,u8 size,u8 mode)	// start x,y, font color, background color, ascii-code, font size, (0)overlying/(1)non-overlying
{
    u8 temp;
    u8 pos,t;
	u16 colortemp=POINT_COLOR;

	num=num-' '; //offset
	LCD_SetWindows(x,y,x+size/2-1,y+size-1);//1개 텍스트 윈도우 설정
	if(!mode) //non-overlying
	{
		for(pos=0;pos<size;pos++)
		{
			// if(size==12)temp=asc2_1206[num][pos];
			// else
			temp=asc2_1608[num][pos];
			for(t=0;t<size/2;t++)
		    {
		        if(temp&0x01)Lcd_WriteData_16Bit(fc);
				else Lcd_WriteData_16Bit(bc);
				temp>>=1;

		    }

		}
	}else //overlying
	{
		for(pos=0;pos<size;pos++)
		{
			// if(size==12)temp=asc2_1206[num][pos];
			// else
			temp=asc2_1608[num][pos];
			for(t=0;t<size/2;t++)
		    {
				POINT_COLOR=fc;
		        if(temp&0x01)LCD_DrawPoint(x+t,y+pos);//점 찍기
		        temp>>=1;
		    }
		}
	}
	POINT_COLOR=colortemp;
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
}

void LCD_ShowString(u16 x,u16 y,u8 size,u8 *p,u8 mode)	// start x,y, font size, string pointer, (0)overlying/(1)non-overlying
{
    while((*p<='~')&&(*p>=' ')) //잘못된 문자 판단
    {
		if(x>(lcddev.width-1)||y>(lcddev.height-1)) return;
        LCD_ShowChar(x,y,POINT_COLOR,BACK_COLOR,*p,size,mode);
        x+=size/2;
        p++;
    }
}

void Draw_LeftArrow(void)
{
	Fill_Triangel(LEFT_X, LEFT_Y+ARROW_SIZE/2, LEFT_X+ARROW_SIZE/2, LEFT_Y, LEFT_X+ARROW_SIZE/2, LEFT_Y+ARROW_SIZE);
	LCD_DrawFillRectangle(LEFT_X+ARROW_SIZE/2+1, LEFT_Y+ARROW_SIZE/4, LEFT_X+ARROW_SIZE, LEFT_Y+ARROW_SIZE*3/4-1);
}

void Draw_RightArrow(void)
{
	Fill_Triangel(RIGHT_X+ARROW_SIZE, RIGHT_Y+ARROW_SIZE/2, RIGHT_X+ARROW_SIZE/2, RIGHT_Y, RIGHT_X+ARROW_SIZE/2, RIGHT_Y+ARROW_SIZE);
	LCD_DrawFillRectangle(RIGHT_X, RIGHT_Y+ARROW_SIZE/4, RIGHT_X+ARROW_SIZE/2, RIGHT_Y+ARROW_SIZE*3/4-1);
}

void Draw_Arrow(void)
{
	if (L_LED_CHECK_ON) POINT_COLOR = DARKGREEN;
	else POINT_COLOR = DARKGRAY;
	Draw_LeftArrow();
	if (R_LED_CHECK_ON) POINT_COLOR = DARKGREEN;
	else POINT_COLOR = DARKGRAY;
	Draw_RightArrow();

}

void Draw_Emergency(void)
{
	if (EMERGENCY) POINT_COLOR = RED;
	else POINT_COLOR = DARKGRAY;

	u8 i;
	for (i = 0; i <= METER_Z; i++)
	{
		Draw_Triangel(EMER_X + ARROW_SIZE/2, EMER_Y+i, EMER_X+i, EMER_Y + ARROW_SIZE-i, EMER_X + ARROW_SIZE-i, EMER_Y + ARROW_SIZE-i);
		Draw_Triangel(EMER_X + ARROW_SIZE/2, EMER_Y+i+METER_Z*4, EMER_X+i+METER_Z*3, EMER_Y + ARROW_SIZE-i - METER_Z*2, EMER_X + ARROW_SIZE-i - METER_Z*3, EMER_Y + ARROW_SIZE-i - METER_Z*2);
	}
}

void Draw_monoBMP(u16 x, u16 y, u16 size_x, u16 size_y, u16 fc, const unsigned char * img, u8 grad)
{
	u16 i;
	u8 j, k, l;
	u16 x0=x;
	POINT_COLOR=fc;
	LCD_SetWindows(x,y,x+size_x*METER_Z-1,y+size_y*METER_Z-1);
	for(i=0;i<size_x*size_y/8;i++)
	{
		for(j=0;j<8;j++)
		{
			if(img[i]&(0x80>>j))
			{
				for (k = 0; k < METER_Z; k++)
				{
					for (l = 0; l < METER_Z; l++)
					{
						LCD_DrawPoint(x+k,y+l);
					}
				}
			}
			x+=METER_Z;

			if((x-x0)==size_x*METER_Z)
			{
				x=x0;
				y+=METER_Z;
				if (grad > 0 && y%(grad*METER_Z) == 0) POINT_COLOR -= 0x0841;
				break;
			}
		}
	}
	LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
}

void Draw_SpeedGage(void)
{
	POINT_COLOR = METER_BACK_COLOR;
	if (SPEED != -1 || SPEED != -2) Fill_Triangel(R_X, R_Y, METER_CENTER_X-METER_PIN_OFFSET, METER_CENTER_Y, METER_CENTER_X, METER_CENTER_Y+METER_PIN_OFFSET);	// R
	if (SPEED != 0) Fill_Triangel(ZERO_X, ZERO_Y, METER_CENTER_X-METER_PIN_OFFSET*0.71, METER_CENTER_Y-METER_PIN_OFFSET*0.71, METER_CENTER_X-METER_PIN_OFFSET*0.71, METER_CENTER_Y+METER_PIN_OFFSET*0.71);	// 0
	if (SPEED != 1) Fill_Triangel(ONE_X, ONE_Y, METER_CENTER_X, METER_CENTER_Y-METER_PIN_OFFSET, METER_CENTER_X-METER_PIN_OFFSET, METER_CENTER_Y);	// 1
	if (SPEED != 2) Fill_Triangel(TWO_X, TWO_Y, METER_CENTER_X-METER_PIN_OFFSET*0.71, METER_CENTER_Y-METER_PIN_OFFSET*0.71, METER_CENTER_X+METER_PIN_OFFSET*0.71, METER_CENTER_Y-METER_PIN_OFFSET*0.71);	// 2
	if (SPEED != 3) Fill_Triangel(THREE_X, THREE_Y, METER_CENTER_X, METER_CENTER_Y-METER_PIN_OFFSET, METER_CENTER_X+METER_PIN_OFFSET, METER_CENTER_Y);	// 3
	if (SPEED != 4) Fill_Triangel(FOUR_X, FOUR_Y, METER_CENTER_X+METER_PIN_OFFSET*0.71+1, METER_CENTER_Y-METER_PIN_OFFSET*0.71, METER_CENTER_X+METER_PIN_OFFSET*0.71+1, METER_CENTER_Y+METER_PIN_OFFSET*0.71);	// 4
	if (SPEED != 5) Fill_Triangel(FIVE_X, FIVE_Y, METER_CENTER_X, METER_CENTER_Y+METER_PIN_OFFSET, METER_CENTER_X+METER_PIN_OFFSET, METER_CENTER_Y);	// 5

	POINT_COLOR = RED;
	if (SPEED == -1 || SPEED == -2) Fill_Triangel(R_X, R_Y, METER_CENTER_X-METER_PIN_OFFSET, METER_CENTER_Y, METER_CENTER_X, METER_CENTER_Y+METER_PIN_OFFSET);	// R
	if (SPEED == 0) Fill_Triangel(ZERO_X, ZERO_Y, METER_CENTER_X-METER_PIN_OFFSET*0.71, METER_CENTER_Y-METER_PIN_OFFSET*0.71, METER_CENTER_X-METER_PIN_OFFSET*0.71, METER_CENTER_Y+METER_PIN_OFFSET*0.71);	// 0
	if (SPEED == 1) Fill_Triangel(ONE_X, ONE_Y, METER_CENTER_X, METER_CENTER_Y-METER_PIN_OFFSET, METER_CENTER_X-METER_PIN_OFFSET, METER_CENTER_Y);	// 1
	if (SPEED == 2) Fill_Triangel(TWO_X, TWO_Y, METER_CENTER_X-METER_PIN_OFFSET*0.71, METER_CENTER_Y-METER_PIN_OFFSET*0.71, METER_CENTER_X+METER_PIN_OFFSET*0.71, METER_CENTER_Y-METER_PIN_OFFSET*0.71);	// 2
	if (SPEED == 3) Fill_Triangel(THREE_X, THREE_Y, METER_CENTER_X, METER_CENTER_Y-METER_PIN_OFFSET, METER_CENTER_X+METER_PIN_OFFSET, METER_CENTER_Y);	// 3
	if (SPEED == 4) Fill_Triangel(FOUR_X, FOUR_Y, METER_CENTER_X+METER_PIN_OFFSET*0.71+1, METER_CENTER_Y-METER_PIN_OFFSET*0.71, METER_CENTER_X+METER_PIN_OFFSET*0.71+1, METER_CENTER_Y+METER_PIN_OFFSET*0.71);	// 4
	if (SPEED == 5) Fill_Triangel(FIVE_X, FIVE_Y, METER_CENTER_X, METER_CENTER_Y+METER_PIN_OFFSET, METER_CENTER_X+METER_PIN_OFFSET, METER_CENTER_Y);	// 5

}

void Draw_Frontsensor(void)
{
	if (FRONT_STATE) POINT_COLOR = RED;
	else POINT_COLOR = DARKGRAY;

	Draw_monoBMP(TOPVIEW_X, TOPVIEW_Y - 8*METER_Z, 24, 6, POINT_COLOR, Img_Wall, 0);
}

void Draw_Rearsensor(void)
{
	if (REAR_STATE) POINT_COLOR = RED;
	else POINT_COLOR = DARKGRAY;

	Draw_monoBMP(TOPVIEW_X, TOPVIEW_Y + TOPVIEW_H + 2*METER_Z, 24, 6, POINT_COLOR, Img_Wall, 0);
}