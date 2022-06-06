
/* ui_ap.c
 *
 *  Created on: 29/May/2022
 *      Author: C. Alvarado
 */

#include "WidgetConfig.h"

#include "Widget.h"
#include "fonts.h"

// tmp header
#include "lcd_log.h"

ButtonIcon mbutton = {
	NULL, NULL, UI_MBUTTON_EVENT
};

ButtonIcon lbutton = {
	NULL, NULL, UI_LBUTTON_EVENT
};

ButtonIcon rbutton = {
	NULL, NULL, UI_RBUTTON_EVENT
};

ButtonIcon tbutton = {
	NULL, NULL, UI_TBUTTON_EVENT
};

ButtonIcon dbutton = {
	NULL, NULL, UI_DBUTTON_EVENT
};


Text app_mode = {
	// &Font12x12, UI_YELLOW_COLOR
	&Font16x24, UI_YELLOW_COLOR
	// &Font8x8, UI_YELLOW_COLOR
};


Widget weather_ui[NUMWIDGETS] = {
	// Pos Y, pos X, dim x, dim y, type, pointer of the text
	// {UI_MBUTTON_YPOS - UI_MBUTTON_RADIUS, UI_MBUTTON_XPOS - UI_MBUTTON_RADIUS,  2*UI_MBUTTON_RADIUS, 2*UI_MBUTTON_RADIUS, BUTTONICON, (void *) &mbutton},
	// {UI_LBUTTON_YPOS, UI_LBUTTON_XPOS, UI_LBUTTON_SIDE, UI_LBUTTON_SIDE, BUTTONICON, (void *) &lbutton},
	// {UI_RBUTTON_YPOS, UI_RBUTTON_XPOS, UI_RBUTTON_SIDE, UI_RBUTTON_SIDE, BUTTONICON, (void *) &rbutton},
	// {UI_TBUTTON_YPOS, UI_TBUTTON_XPOS, UI_TBUTTON_BASE, UI_TBUTTON_HEIGHT, BUTTONICON, (void *) &tbutton},
	// {UI_DBUTTON_YPOS, UI_DBUTTON_XPOS, UI_DBUTTON_BASE, UI_DBUTTON_HEIGHT, BUTTONICON, (void *) &dbutton},
	{UI_MBUTTON_XPOS - UI_MBUTTON_RADIUS, UI_MBUTTON_YPOS - UI_MBUTTON_RADIUS, 2*UI_MBUTTON_RADIUS, 2*UI_MBUTTON_RADIUS, BUTTONICON, (void *) &mbutton},
	{UI_LBUTTON_XPOS, UI_LBUTTON_YPOS, UI_LBUTTON_SIDE, UI_LBUTTON_SIDE, BUTTONICON, (void *) &lbutton},
	{UI_RBUTTON_XPOS, UI_RBUTTON_YPOS, UI_RBUTTON_SIDE, UI_RBUTTON_SIDE, BUTTONICON, (void *) &rbutton},
	{UI_TBUTTON_XPOS, UI_TBUTTON_YPOS, UI_TBUTTON_BASE, UI_TBUTTON_HEIGHT, BUTTONICON, (void *) &tbutton},
	{UI_DBUTTON_XPOS + 20, UI_DBUTTON_YPOS - 40, UI_DBUTTON_BASE, UI_DBUTTON_HEIGHT, BUTTONICON, (void *) &dbutton},
	{120, 5, 8, 8, TEXT, (void *) &app_mode},
};

static void LCD_DrawFullCircle_mine(uint16_t Xpos, uint16_t Ypos, uint16_t Radius)
{
  int32_t  D;    /* Decision Variable */ 
  uint32_t  CurX;/* Current X Value */
  uint32_t  CurY;/* Current Y Value */ 
  
  D = 3 - (Radius << 1);

  CurX = 0;
  CurY = Radius;
  
  while (CurX <= CurY) {
    if(CurY > 0) {
      LCD_DrawLine(Xpos - CurX, Ypos + CurY, 2*CurX, LCD_DIR_HORIZONTAL);
      LCD_DrawLine(Xpos - CurX, Ypos - CurY, 2*CurX, LCD_DIR_HORIZONTAL);
    }
	
    if(CurX >= 0) {
      LCD_DrawLine(Xpos - CurY, Ypos - CurX, 2*CurY, LCD_DIR_HORIZONTAL);
      LCD_DrawLine(Xpos - CurY, Ypos + CurX, 2*CurY, LCD_DIR_HORIZONTAL);
    }
       
    if (D < 0) { 
      D += (CurX << 2) + 6;
    }
    else {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
        
    CurX++;
  }

  LCD_DrawCircle(Xpos, Ypos, Radius);
}

static void LCD_DrawFullIsoscelesTriangle(unsigned short xpos, unsigned short ypos, unsigned short side, unsigned short orientation) {
	unsigned short xcurr = xpos;
	unsigned short ycurr = ypos;

	if (orientation == 3) {
		xcurr = xpos + side;
		while(ycurr <= (ypos + side - 1)) {
			if(ycurr < (ypos + (side >> 1))) xcurr -= 2;
			else xcurr += 2;
			
			LCD_DrawLine(xcurr, ycurr, (xpos + side) - xcurr, LCD_DIR_HORIZONTAL); 
			ycurr++;
		}

	} else if (orientation == 1) {
		xcurr = xpos;
		while(ycurr <= (ypos + side - 1)) {
			if (ycurr < (ypos + (side >> 1))) xcurr += 2;
			else xcurr -= 2;
			
			LCD_DrawLine(xpos, ycurr, xcurr - xpos, LCD_DIR_HORIZONTAL); 
			ycurr++;

		}
	}
}

static void LCD_DrawFullTriangleBaseHight(unsigned short xpos, unsigned short ypos, unsigned short base, short height) {
	unsigned short ycurr = ypos;
	unsigned short xcurr = xpos + (base >> 1);
	unsigned short d = 0;
	if (height > 0) {
		while(ycurr <= (ypos + height)) {
			LCD_DrawLine(xcurr, ycurr, d << 1,  LCD_DIR_HORIZONTAL);
			xcurr--;
			d++;
			ycurr++;
		}
	} else {
			while(ycurr >= (ypos + height)) {
			LCD_DrawLine(xcurr, ycurr, d << 1,  LCD_DIR_HORIZONTAL);
			xcurr--;
			d++;
			ycurr--;
		}
	} 

}

static void UI_DrawNumber7segments(char num, UI_NUM_t ss, unsigned short numcolor) {
	unsigned short i = 0;

	LCD_SetColors(numcolor, numcolor);
	// LCD_SetColors(UI_TOP_COLOR, UI_TOP_COLOR);

	for (i = 0; i < ss.thick; ++i) {
		LCD_DrawRect(ss.xpos + i, ss.ypos + i, ss.width, 
			ss.height);
		LCD_DrawLine(ss.xpos + i, ss.ypos + ss.width + i, 
			ss.width, LCD_DIR_HORIZONTAL);
	}

	LCD_SetColors(UI_TOP_COLOR, UI_TOP_COLOR);

	for (i = 0; i < ss.thick; ++i) {
		if (num == 0) {
			LCD_DrawLine(ss.xpos + ss.thick, ss.ypos + ss.width + i,
				ss.width - ss.thick - 1, LCD_DIR_HORIZONTAL);
		}

		else if (num == 1) {
			LCD_DrawLine(ss.xpos, ss.ypos + ss.width + i, ss.width - 1, 
				LCD_DIR_HORIZONTAL);
			LCD_DrawLine(ss.xpos + i, ss.ypos + i, ss.height, 
				LCD_DIR_VERTICAL);		
			LCD_DrawLine(ss.xpos - 1, ss.ypos + i, ss.width, LCD_DIR_HORIZONTAL);		
			LCD_DrawLine(ss.xpos + i, ss.ypos + i + ss.height, ss.width, 
					LCD_DIR_HORIZONTAL);		
		} 

		else if (num == 2) {		
			LCD_DrawLine(ss.xpos + i, ss.ypos, ss.width, LCD_DIR_VERTICAL);		
			LCD_DrawLine(ss.xpos + i + ss.width - 1, ss.ypos + ss.width + ss.thick, 
				ss.width, LCD_DIR_VERTICAL);		
		} 

		else if (num == 3) {
			LCD_DrawLine(ss.xpos + i, ss.ypos, ss.height, LCD_DIR_VERTICAL);		
		}

		else if (num == 4) {
			LCD_DrawLine(ss.xpos + i, ss.ypos + i + ss.width, ss.width, 
				LCD_DIR_VERTICAL);		
			LCD_DrawLine(ss.xpos - 1, ss.ypos + i, ss.width, LCD_DIR_HORIZONTAL);
			LCD_DrawLine(ss.xpos + i, ss.ypos + i + ss.height, ss.width,
				LCD_DIR_HORIZONTAL);				
		} 

		else if (num == 5) {		
			LCD_DrawLine(ss.xpos + i + ss.width - 1, ss.ypos + i, ss.width, 
				LCD_DIR_VERTICAL);		
			LCD_DrawLine(ss.xpos + i, ss.ypos + ss.width + i, ss.width, 
				LCD_DIR_VERTICAL);		

		}

		else if (num == 6) {		
			LCD_DrawLine(ss.xpos + i + ss.width - 1, ss.ypos + i, ss.width,
				LCD_DIR_VERTICAL);		
		} 

		else if (num == 7) {
			LCD_DrawLine(ss.xpos, ss.ypos + ss.width + i, ss.width - 1, 
				LCD_DIR_HORIZONTAL);
			LCD_DrawLine(ss.xpos + i, ss.ypos + i, ss.height, 
				LCD_DIR_VERTICAL);		
			LCD_DrawLine(ss.xpos + i, ss.ypos + i + ss.height, ss.width,
				LCD_DIR_HORIZONTAL);		
		} 

		else if (num == 9) {		
			LCD_DrawLine(ss.xpos + i, ss.ypos + ss.width + i, ss.width, LCD_DIR_VERTICAL);
		}
	}

}

static void numChar2DecNUnit(char num, char *dec, char * units) {
	*dec = num / 10;
	*units = num - (*dec) * 10;
}

static void UI_DrawDecNumber(UI_NUM_t timex, unsigned short color) {
	char dec, units;
	numChar2DecNUnit(timex.value, &dec, &units);

	UI_DrawNumber7segments(dec, timex, color);
	timex.xpos = ((timex.xpos << 2) + timex.width * 5) >> 2;
	UI_DrawNumber7segments(units, timex, color);
}



void UI_DrawFixElements(void) {
	LCD_SetColors(UI_TOP_COLOR, UI_TOP_COLOR);
	LCD_DrawFullRect(0, 0, 320, 163);
	LCD_SetColors(UI_NUMBERS_COLOR, UI_NUMBERS_COLOR);

	// Hour
	widget_time[HOURS_STR].xpos = 10;
	widget_time[HOURS_STR].ypos = 22;
	widget_time[HOURS_STR].width = 40;
	widget_time[HOURS_STR].height = 74;
	widget_time[HOURS_STR].thick = 6;
	widget_time[HOURS_STR].value = 11;

	// Minutes
	widget_time[MINUTES_STR].xpos = 130;
	widget_time[MINUTES_STR].ypos = 22;
	widget_time[MINUTES_STR].width = 40;
	widget_time[MINUTES_STR].height = 74;
	widget_time[MINUTES_STR].thick = 6;
	widget_time[MINUTES_STR].value = 1;

	// Seconds	
	widget_time[SECONDS_STR].xpos = 250;
	widget_time[SECONDS_STR].ypos = 50;
	widget_time[SECONDS_STR].width = 26;
	widget_time[SECONDS_STR].height = 48;
	widget_time[SECONDS_STR].thick = 4;
	widget_time[SECONDS_STR].value = 59;

	// Main button
	LCD_DrawFullCircle_mine(UI_MBUTTON_XPOS, UI_MBUTTON_YPOS, 
		UI_MBUTTON_RADIUS);
	// Left button
	LCD_DrawFullIsoscelesTriangle(UI_LBUTTON_XPOS, UI_RBUTTON_YPOS, 
		UI_LBUTTON_SIDE, UI_LBUTTON_ORIENTATION);
	// Right button
	LCD_DrawFullIsoscelesTriangle(UI_RBUTTON_XPOS, UI_RBUTTON_YPOS, 
		UI_RBUTTON_SIDE, UI_RBUTTON_ORIENTATION);

	LCD_SetColors(UI_YELLOW_COLOR, UI_YELLOW_COLOR);
	// ':'' for HH:MM
	LCD_DrawFullRect(114, 51, 7, 7);
	LCD_DrawFullRect(114, 71, 7, 7);
	// ':' for MM:SS
	LCD_DrawFullRect(234, 51, 7, 7);
	LCD_DrawFullRect(234, 71, 7, 7);

	LCD_SetColors(APP_BACKGROUND_COLOR, APP_BACKGROUND_COLOR);
	// Up button
	LCD_DrawFullTriangleBaseHight(UI_TBUTTON_XPOS, UI_TBUTTON_YPOS, 
		UI_TBUTTON_BASE, UI_TBUTTON_HEIGHT);

	// Down button
	LCD_DrawFullTriangleBaseHight(UI_DBUTTON_XPOS, UI_DBUTTON_YPOS, 
		UI_DBUTTON_BASE, -1*UI_DBUTTON_HEIGHT);
}

void UI_WriteTime(char timeVal, char timeUnit, unsigned short color) {
	widget_time[timeUnit].value = timeVal;
	UI_DrawDecNumber(widget_time[timeUnit], color);
}
