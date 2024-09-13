/*
 * app.c
 *
 *  Created on: 29/May/2022
 *      Author: c. Alvarado
 */

#include "app.h"
#include "WidgetConfig.h"

static App_t m_app;

TimeState_t currentTime;
TimeState_t stopwatchTime;
TimeState_t alarmTime;
WatchTime_t alarmTime_conf;
WatchTime_t currentTime_conf;

static void time_FSM (TimeState_t *tmw, char maxHour, char show) {
	tmw->st = MILISECONDS_STATE;

	if(100 == m_app.tick)
		tmw->st = SECONDS_STATE;

	if (SECONDS_STATE == tmw->st) {
		if (24 == maxHour) m_app.tick = 0;
		
		tmw->tm.seconds++;
		if (60 == tmw->tm.seconds) {
			tmw->tm.seconds = 0;
			tmw->st = MINUTES_STATE;
		}

		if(show)
			UI_WriteTime(tmw->tm.seconds, SECONDS_STR, UI_NUMBERS_COLOR);
	}

	if(MINUTES_STATE == tmw->st) {
		tmw->tm.minutes++;

		if (60 == tmw->tm.minutes) {
			tmw->tm.minutes = 0;
			tmw->st = HOURS_STATE;
		}

		if(show)
			UI_WriteTime(tmw->tm.minutes, MINUTES_STR, UI_NUMBERS_COLOR);
	}

	if(HOURS_STATE == tmw->st) {
		tmw->tm.hours++;
		if (maxHour == tmw->tm.hours)
			tmw->tm.hours = 0;

		if(show)
			UI_WriteTime(tmw->tm.hours, HOURS_STR, UI_NUMBERS_COLOR);

	}
}

static void updateTimeOnScreen(WatchTime_t tm, unsigned char active) {
	unsigned short color;

	color = HOURS_STR == active ? UI_YELLOW_COLOR : UI_NUMBERS_COLOR;
	UI_WriteTime(tm.hours, HOURS_STR, color);

	color = MINUTES_STR == active ? UI_YELLOW_COLOR : UI_NUMBERS_COLOR;
	UI_WriteTime(tm.minutes, MINUTES_STR, color);

	if (active == SECONDS_STR)
		UI_WriteTime(tm.seconds, SECONDS_STR, UI_YELLOW_COLOR);
	else
		UI_WriteTime(tm.seconds, SECONDS_STR, UI_NUMBERS_COLOR);
}

static void time_config(WatchTime_t *wt_conf, char updown) {
	switch(m_app.timeUnit2Config) {
	case HOURS_STATE:
		wt_conf->hours += updown;
		
		if (24 == wt_conf->hours)
			wt_conf->hours = 0;
		else if (255 == wt_conf->hours)
			wt_conf->hours = 23;

		return;

	case MINUTES_STATE:
		wt_conf->minutes += updown;

		if (60 == wt_conf->minutes)
			wt_conf->minutes = 0;
		else if (255 == wt_conf->minutes) 
			wt_conf->minutes = 59;

		return;	

	case SECONDS_STATE:
		wt_conf->seconds += updown;
		
		if (60 == wt_conf->seconds)
			wt_conf->seconds = 0;
		else if (255 == wt_conf->seconds)
			wt_conf->seconds = 59;

		return;	
	}
}


static inline void HOME_STATE_function(char button_pressed) {
	switch(button_pressed) {
	case UI_RBUTTON_EVENT:
		m_app.mode = STOPWATCH_STATE;
		updateTimeOnScreen(stopwatchTime.tm, TIME_UNIT_NONE_STR);
		UI_WriteMode(STOPWATCH_STR);
		return;

	case UI_LBUTTON_EVENT:
		m_app.mode = TIME_SET_STATE;
		currentTime_conf = currentTime.tm;
		updateTimeOnScreen(currentTime_conf, TIME_UNIT_NONE_STR);
		UI_WriteMode(TIMESET_STR);
	}

}

static inline void STOPWATCH_STATE_function(char button_pressed) {
		
	switch(button_pressed) {
	case UI_MBUTTON_EVENT:
		m_app.swatchStatus = SWATCH_START == m_app.swatchStatus
							? SWATCH_PAUSE 
							: SWATCH_START;
		return;
	
	case UI_TBUTTON_EVENT:
		stopwatchTime.tm.seconds = 0;
		stopwatchTime.tm.minutes = 0;
		stopwatchTime.tm.hours = 0;
		updateTimeOnScreen(stopwatchTime.tm, TIME_UNIT_NONE_STR);
		return;
	
	case UI_RBUTTON_EVENT:
		m_app.mode = ALARM_STATE;
		UI_WriteMode(ALARM_STR);
		updateTimeOnScreen(alarmTime.tm, TIME_UNIT_NONE_STR);
		return;
	
	case UI_LBUTTON_EVENT:
		m_app.mode = HOME_STATE;
		UI_WriteMode(TIME_STR);
		updateTimeOnScreen(currentTime.tm, TIME_UNIT_NONE_STR);
	}
}

static inline void ALARM_STATE_functions(char button_pressed) {
	
	switch(button_pressed) {
	case UI_MBUTTON_EVENT:
		m_app.mode = ALARM_STATE_2;
		m_app.timeUnit2Config = HOURS_STATE;
		alarmTime_conf = alarmTime.tm;
		m_app.alarmStatus = ALARM_DISABLED;
		UI_WriteMode(ALARM_CON_STR);
		updateTimeOnScreen(alarmTime.tm, m_app.timeUnit2Config);
		return;

	case UI_RBUTTON_EVENT:
		m_app.mode = TIME_SET_STATE;
		currentTime_conf = currentTime.tm;
		UI_WriteMode(TIMESET_STR);
		updateTimeOnScreen(currentTime_conf, TIME_UNIT_NONE_STR);
		return;

	case UI_LBUTTON_EVENT:
		m_app.mode = STOPWATCH_STATE;
		UI_WriteMode(STOPWATCH_STR);
		updateTimeOnScreen(stopwatchTime.tm, TIME_UNIT_NONE_STR);
	}

}

static inline void ALARM_STATE_2_functions(char button_pressed) {
	switch(button_pressed) {
	case UI_MBUTTON_EVENT:
		m_app.mode = ALARM_STATE;
		updateTimeOnScreen(alarmTime.tm, TIME_UNIT_NONE_STR);
		UI_WriteMode(ALARM_STR);
		return;

	case UI_TBUTTON_EVENT:
		if (ALARM_DISABLED == m_app.alarmStatus)
			time_config(&alarmTime_conf, 1);

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);
		return;

	case UI_DBUTTON_EVENT:
		if (ALARM_DISABLED == m_app.alarmStatus)
			time_config(&alarmTime_conf, -1);

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);
		return;

	case UI_RBUTTON_EVENT:
		m_app.timeUnit2Config++;

		if(m_app.timeUnit2Config > SECONDS_STATE) {					
			if (ALARM_ENABLED == m_app.alarmStatus) {
				m_app.alarmStatus = ALARM_DISABLED;
				UI_WriteMode(ALARM_CON_STR);
			}

			else if (ALARM_DISABLED == m_app.alarmStatus) {
				alarmTime.tm = alarmTime_conf;
				m_app.alarmStatus = ALARM_ENABLED;
				UI_WriteMode(ALARM_SET_STR);
			}
			m_app.timeUnit2Config = HOURS_STR;
		}

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);
		return;


	case UI_LBUTTON_EVENT:
		m_app.timeUnit2Config--;

		if(255 == m_app.timeUnit2Config)
			m_app.timeUnit2Config = SECONDS_STR;

		updateTimeOnScreen(alarmTime_conf, m_app.timeUnit2Config);
	}	
}

static inline void TIME_SET_STATE_function(char button_pressed) {
	switch(button_pressed) {
	case UI_MBUTTON_EVENT:
		m_app.mode = TIME_SET_STATE_2;
		m_app.timeUnit2Config = HOURS_STATE;
		UI_WriteMode(TIMESET2_STR);
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);
		return;

	case UI_RBUTTON_EVENT:
		m_app.mode = HOME_STATE;
		UI_WriteMode(TIME_STR);
		updateTimeOnScreen(currentTime.tm, TIME_UNIT_NONE_STR);
		return;
	
	case UI_LBUTTON_EVENT:
		m_app.mode = ALARM_STATE;
		UI_WriteMode(ALARM_STR);
		updateTimeOnScreen(alarmTime.tm, TIME_UNIT_NONE_STR);
	}
}

static inline void TIME_SET_STATE_2_function(char button_pressed) {
	switch(button_pressed) {
	case UI_MBUTTON_EVENT:
		m_app.mode = TIME_SET_STATE;
		currentTime.tm = currentTime_conf;
		UI_WriteMode(TIMESET_STR);
		return;

	case UI_TBUTTON_EVENT:
		time_config(&currentTime_conf, 1);
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);
		return;

	case UI_DBUTTON_EVENT:
		time_config(&currentTime_conf, -1);
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);
		return;

	case UI_RBUTTON_EVENT:
		m_app.timeUnit2Config++;
		if (m_app.timeUnit2Config > SECONDS_STR)
			m_app.timeUnit2Config = HOURS_STR;
		updateTimeOnScreen(currentTime_conf, m_app.timeUnit2Config);
		return;
 				
	case UI_LBUTTON_EVENT:
		m_app.timeUnit2Config--;
		if (255 == m_app.timeUnit2Config)
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
			if (0 == period%50) {
				blink ^= 1;
				if (blink)
					UI_WriteMode(ALARM_REACHED_STR);
				else 
					UI_WriteMode("      ");
			}
			state = 1;
			break;
		case 1:
			UI_WriteMode(TIME_STR);
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
	UI_WriteMode(TIME_STR);
}

void app(void) {
	mode_FSM();

	if (m_app.swatchStatus != SWATCH_PAUSE)
		time_FSM(&stopwatchTime, 99, STOPWATCH_STATE == m_app.mode);

	time_FSM(&currentTime, 24, HOME_STATE == m_app.mode);
	if (ALARM_ENABLED == m_app.alarmStatus) 
		time_compareAlarm(currentTime.tm, alarmTime.tm);

	m_app.tick++;
}

