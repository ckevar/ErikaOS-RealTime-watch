/*
 * app.c
 *
 *  Created on: 29/May/2022
 *      Author: c. Alvarado
 */

#include "app.h"
#include "WidgetConfig.h"

// tmp header
#include "lcd_log.h"

static App_t m_app;

static void time_FSM (TimeState_t *tmw, char maxHour, char show) {
	tmw->st = MILISECONDS_STATE;

	if(m_app.tick == 100)
		tmw->st = SECONDS_STATE;

	if (tmw->st == SECONDS_STATE) {
		if (maxHour == 24) m_app.tick = 0;
		
		tmw->tm.seconds++;
		if (tmw->tm.seconds == 60) {
			tmw->tm.seconds = 0;
			tmw->st = MINUTES_STATE;
		}

		if(show)
			UI_WriteTime(tmw->tm.seconds, SECONDS_STR, UI_NUMBERS_COLOR);
	}

	if(tmw->st == MINUTES_STATE) {
		tmw->tm.minutes++;

		if (tmw->tm.minutes == 60) {
			tmw->tm.minutes = 0;
			tmw->st = HOURS_STATE;
		}

		if(show)
			UI_WriteTime(tmw->tm.minutes, MINUTES_STR, UI_NUMBERS_COLOR);
	}

	if(tmw->st == HOURS_STATE) {
		tmw->tm.hours++;
		if (tmw->tm.hours == maxHour)
			tmw->tm.hours = 0;

		if(show)
			UI_WriteTime(tmw->tm.hours, HOURS_STR, UI_NUMBERS_COLOR);

	}
}

static void updateTimeOnScreen(WatchTime_t tm, unsigned char active) {
	if (active == HOURS_STR) {
		UI_WriteTime(tm.hours, HOURS_STR, UI_YELLOW_COLOR);
	}
	else 
		UI_WriteTime(tm.hours, HOURS_STR, UI_NUMBERS_COLOR);

	if (active == MINUTES_STR)
		UI_WriteTime(tm.minutes, MINUTES_STR, UI_YELLOW_COLOR);
	else 
		UI_WriteTime(tm.minutes, MINUTES_STR, UI_NUMBERS_COLOR);

	if (active == SECONDS_STR)
		UI_WriteTime(tm.seconds, SECONDS_STR, UI_YELLOW_COLOR);
	else
		UI_WriteTime(tm.seconds, SECONDS_STR, UI_NUMBERS_COLOR);
}

static void time_config(WatchTime_t *wt_conf, char updown) {
	if (m_app.timeUnit2Config == HOURS_STATE) {
		wt_conf->hours += updown;
		if (wt_conf->hours == 24)
			wt_conf->hours = 0;
		else if (wt_conf->hours == 255) 
			wt_conf->hours = 23;
		return;
	}
	if (m_app.timeUnit2Config == MINUTES_STATE) {
		wt_conf->minutes += updown;
		if (wt_conf->minutes == 60)
			wt_conf->minutes = 0;
		else if (wt_conf->minutes == 255) 
			wt_conf->minutes = 59;
		return;	
	}
	if (m_app.timeUnit2Config == SECONDS_STATE) {
		wt_conf->seconds += updown;
		if (wt_conf->seconds == 60)
			wt_conf->seconds = 0;
		else if (wt_conf->seconds == 255)
			wt_conf->seconds = 59;
		return;	
	}
}


static inline void HOME_STATE_function(char button_pressed) {
	if (button_pressed == UI_RBUTTON_EVENT) {
		m_app.mode = STOPWATCH_STATE;
		updateTimeOnScreen(stopwatchTime.tm, TIME_UNIT_NONE_STR);
		WPrint(&weather_ui[5], STOPWATCH_STR);
	}
	else if (button_pressed == UI_LBUTTON_EVENT) {
		m_app.mode = TIME_SET_STATE;
		currentTime_conf = currentTime.tm;
		updateTimeOnScreen(currentTime_conf, TIME_UNIT_NONE_STR);
		WPrint(&weather_ui[5], TIMESET_STR);
	}

}

static inline void STOPWATCH_STATE_function(char button_pressed) {
	// LCD_UsrLog("bp %d\r\n", button_pressed);	
	if (button_pressed == UI_MBUTTON_EVENT) {
		m_app.swatchStatus = (m_app.swatchStatus == SWATCH_START) ? SWATCH_PAUSE : SWATCH_START;
	
	} else if (button_pressed == UI_TBUTTON_EVENT) {
			stopwatchTime.tm.seconds = 0;
			stopwatchTime.tm.minutes = 0;
			stopwatchTime.tm.hours = 0;
			updateTimeOnScreen(stopwatchTime.tm, TIME_UNIT_NONE_STR);
	
	} else if (button_pressed == UI_RBUTTON_EVENT) {
		m_app.mode = ALARM_STATE;
		WPrint(&weather_ui[5], ALARM_STR);
		updateTimeOnScreen(alarmTime.tm, TIME_UNIT_NONE_STR);
	
	} else if (button_pressed == UI_LBUTTON_EVENT) {
		m_app.mode = HOME_STATE;
		WPrint(&weather_ui[5], TIME_STR);
		updateTimeOnScreen(currentTime.tm, TIME_UNIT_NONE_STR);
	}
}

static inline void ALARM_STATE_functions(char button_pressed) {

	if (button_pressed == UI_MBUTTON_EVENT) {
		m_app.mode = ALARM_STATE_2;
		m_app.timeUnit2Config = HOURS_STATE;
		alarmTime_conf = alarmTime.tm;
		m_app.alarmStatus = ALARM_DISABLED;
		WPrint(&weather_ui[5], ALARM_CON_STR);
		updateTimeOnScreen(alarmTime.tm, m_app.timeUnit2Config);

	} else if (button_pressed == UI_RBUTTON_EVENT) {
		m_app.mode = TIME_SET_STATE;
		currentTime_conf = currentTime.tm;
		WPrint(&weather_ui[5], TIMESET_STR);
		updateTimeOnScreen(currentTime_conf, TIME_UNIT_NONE_STR);

	} else if (button_pressed == UI_LBUTTON_EVENT){
		m_app.mode = STOPWATCH_STATE;
		WPrint(&weather_ui[5], STOPWATCH_STR);
		updateTimeOnScreen(stopwatchTime.tm, TIME_UNIT_NONE_STR);
	}

}

static inline void ALARM_STATE_2_functions(char button_pressed) {
	if (button_pressed == UI_MBUTTON_EVENT) {
		m_app.mode = ALARM_STATE;
		updateTimeOnScreen(alarmTime.tm, TIME_UNIT_NONE_STR);
		WPrint(&weather_ui[5], ALARM_STR);

	} else if (button_pressed == UI_TBUTTON_EVENT) {
		if (m_app.alarmStatus == ALARM_DISABLED)
			time_config(&alarmTime_conf, 1);

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);

	} else if (button_pressed == UI_DBUTTON_EVENT) {
		if (m_app.alarmStatus == ALARM_DISABLED)
			time_config(&alarmTime_conf, -1);

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);

	} else if (button_pressed == UI_RBUTTON_EVENT) {
		m_app.timeUnit2Config++;

		if(m_app.timeUnit2Config > SECONDS_STATE) {					
			if (m_app.alarmStatus == ALARM_ENABLED) {
				m_app.alarmStatus = ALARM_DISABLED;
				WPrint(&weather_ui[5], ALARM_CON_STR);
			}

			else if (m_app.alarmStatus == ALARM_DISABLED) {
				alarmTime.tm = alarmTime_conf;
				m_app.alarmStatus = ALARM_ENABLED;
				WPrint(&weather_ui[5], ALARM_SET_STR);
			}
			m_app.timeUnit2Config = HOURS_STR;
		}

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);


	} else if (button_pressed == UI_LBUTTON_EVENT) {
		m_app.timeUnit2Config--;

		if(m_app.timeUnit2Config == 255)
			m_app.timeUnit2Config = SECONDS_STR;

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);
	}	
}

static inline void TIME_SET_STATE_function(char button_pressed) {
	if (button_pressed == UI_MBUTTON_EVENT) {
		m_app.mode = TIME_SET_STATE_2;
		m_app.timeUnit2Config = HOURS_STATE;
		WPrint(&weather_ui[5], TIMESET2_STR);
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);

	} else if (button_pressed == UI_RBUTTON_EVENT) {
		m_app.mode = HOME_STATE;
		WPrint(&weather_ui[5], TIME_STR);
		updateTimeOnScreen(currentTime.tm, TIME_UNIT_NONE_STR);
	
	} else if (button_pressed == UI_LBUTTON_EVENT) {
		m_app.mode = ALARM_STATE;
		WPrint(&weather_ui[5], ALARM_STR);
		updateTimeOnScreen(alarmTime.tm, TIME_UNIT_NONE_STR);
	}
}

static inline void TIME_SET_STATE_2_function(char button_pressed) {
	if (button_pressed == UI_MBUTTON_EVENT) {
		m_app.mode = TIME_SET_STATE;
		currentTime.tm = currentTime_conf;
		WPrint(&weather_ui[5], TIMESET_STR);

	} else if (button_pressed == UI_TBUTTON_EVENT) {
		time_config(&currentTime_conf, 1);
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);

	} else if (button_pressed == UI_DBUTTON_EVENT) {
		time_config(&currentTime_conf, -1);
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);

	} else if (button_pressed == UI_RBUTTON_EVENT) {
		m_app.timeUnit2Config++;
		if (m_app.timeUnit2Config > SECONDS_STR)
			m_app.timeUnit2Config = HOURS_STR;
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);
 				
	} else if (button_pressed == UI_LBUTTON_EVENT) {
		m_app.timeUnit2Config--;
		if (m_app.timeUnit2Config == 255)
			m_app.timeUnit2Config = SECONDS_STR;
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);
	}
}

static void mode_FSM_event(char button_pressed) {

	switch (m_app.mode) {
		case HOME_STATE:
			HOME_STATE_function(button_pressed);		
			break;

		case STOPWATCH_STATE:
			STOPWATCH_STATE_function(button_pressed);
			break;

		case ALARM_STATE:
			ALARM_STATE_functions(button_pressed);
			break;

		case ALARM_STATE_2:
			ALARM_STATE_2_functions(button_pressed);
			break;

		case TIME_SET_STATE:
			TIME_SET_STATE_function(button_pressed);
			break;

		case TIME_SET_STATE_2:
			TIME_SET_STATE_2_function(button_pressed);
			break;
	}

}

static void mode_FSM(void) {
	char i, BUTTON_EVENT;
	for (i = 1; i <= 0x10; i <<= 1) {
		// LCD_UsrLog("%x\r\n", i);
		BUTTON_EVENT = evts & i;
		if (BUTTON_EVENT)
			mode_FSM_event(BUTTON_EVENT);
	}
	ClearEvents();
}

static char time_compareAlarm(WatchTime_t cwt, WatchTime_t awt) {
	static unsigned short period = 0;
	static char blink = 0; 
	static char state = 2;
	if (cwt.hours == awt.hours) {
		if (cwt.minutes == awt.minutes) {
			if (cwt.seconds == awt.seconds)
				state = 0;
		}
	}
	switch(state) {
		case 0:
			if (period%50 == 0) {
				blink ^= 1;
				if (blink)
					WPrint(&weather_ui[5], ALARM_REACHED_STR);
				else 
					WPrint(&weather_ui[5], "      ");
			}
			state = 1;
			break;
		case 1:
			WPrint(&weather_ui[5], TIME_STR);
			state = 2;
			break;
	}
}


void app_init(void) {
	m_app.mode = HOME_STATE;
	m_app.swatchStatus = SWATCH_PAUSE;
	m_app.alarmStatus = ALARM_DISABLED;
	m_app.tick = 0;

	currentTime.tm.hours = 0;
	currentTime.tm.minutes = 0;
	currentTime.tm.seconds = 0;

	alarmTime.tm.hours = 0;
	alarmTime.tm.minutes = 0;
	alarmTime.tm.seconds = 0;

	updateTimeOnScreen(currentTime.tm, TIME_UNIT_NONE_STR);
	WPrint(&weather_ui[5], TIME_STR);
}

void app(void) {
	mode_FSM();

	if (m_app.swatchStatus != SWATCH_PAUSE)
		time_FSM(&stopwatchTime, 99, m_app.mode == STOPWATCH_STATE);

	time_FSM(&currentTime, 24, m_app.mode == HOME_STATE);
	if (m_app.alarmStatus == ALARM_ENABLED) 
		time_compareAlarm(currentTime.tm, alarmTime.tm);

	m_app.tick++;
}

