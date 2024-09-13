/*
 * WidgetConfig.h
 *
 *  Created on: 22/ott/2015
 *      Author: admim
 * 		Modified: c. Alvarado - May 29, 2022
 */

#ifndef WIDGETCONFIG_H_
#define WIDGETCONFIG_H_

#include "Widget.h"
#include "stm32f4_discovery_lcd.h"


enum {
	LOCATION_IMG,			
	CITY_STR,
	WiFi_IMG,				
	WiFi_AP_SET,
	DESCRIPTION_STR,
	TEMP_DEGREE_STR,
	NUMWIDGETS
};

#define APP_BACKGROUND_COLOR	0x6335
#define UI_TOP_COLOR			0x31AC
#define UI_NUMBERS_COLOR		0xE73c
#define UI_YELLOW_COLOR			0xD6A7

#define HOURS_STR				0
#define MINUTES_STR				1
#define SECONDS_STR				2
#define TIME_UNIT_NONE_STR		4

#define TIMESET_STR				"TM SET"
#define TIMESET2_STR			"Config"
#define ALARM_STR				"ALARM "
#define ALARM_REACHED_STR		"ALARM!"
#define ALARM_CON_STR			"AL Con"
#define ALARM_SET_STR			"AL Set"
#define STOPWATCH_STR			"SWATCH"
#define TIME_STR				"TIME  "

// BUTTONS PARAMETERS
#define UI_MBUTTON_XPOS			160
#define UI_MBUTTON_YPOS			203
#define UI_MBUTTON_RADIUS		20
#define UI_MBUTTON_EVENT		0x01

#define UI_LBUTTON_XPOS			31
#define UI_LBUTTON_YPOS			183
#define UI_LBUTTON_SIDE			40
#define UI_LBUTTON_ORIENTATION 	3
#define UI_LBUTTON_EVENT		0x02

#define UI_RBUTTON_XPOS			248
#define UI_RBUTTON_YPOS			183
#define UI_RBUTTON_SIDE			40
#define UI_RBUTTON_ORIENTATION 	1
#define UI_RBUTTON_EVENT		0x04

#define UI_TBUTTON_XPOS			127
#define UI_TBUTTON_YPOS			112
#define UI_TBUTTON_BASE			66
#define UI_TBUTTON_HEIGHT		35
#define UI_TBUTTON_EVENT		0x08


#define UI_DBUTTON_XPOS			227
#define UI_DBUTTON_YPOS			147
#define UI_DBUTTON_BASE			66
#define UI_DBUTTON_HEIGHT		35
#define UI_DBUTTON_EVENT		0x10

typedef struct {
	unsigned short xpos;
	unsigned short ypos;
	unsigned short width;
	unsigned short height;
	unsigned short thick;
	char value;
} UI_NUM_t;

extern Widget weather_ui[NUMWIDGETS];

void UI_DrawFixElements(void);
void UI_WriteTime(char timeVal, char timeUnit, unsigned short color);
void UI_WriteMode(char *mode);

#endif 
