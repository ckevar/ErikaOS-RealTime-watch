/*
 * app.h
 *
 *  Created on: 29/May/2022
 *      Author: c. Alvarado
 */
#ifndef APP_H
#define APP_H 

/* Typedefs */
typedef enum TimeUnitState_t {
	HOURS_STATE = 0,
	MINUTES_STATE = 1, 
	SECONDS_STATE = 2, 
	MILISECONDS_STATE = 3 
} TimeUnitState_t;

typedef enum ModeState_t {
	HOME_STATE,
	STOPWATCH_STATE,
	ALARM_STATE,
	ALARM_STATE_2,
	TIME_SET_STATE,
	TIME_SET_STATE_2
} ModeState_t;

typedef enum StopwatchStatus_t
{
	SWATCH_START,
	SWATCH_PAUSE,
	SWATCH_RESET,
} StopwatchStatus_t;

typedef enum timeSetStatus_t {
	ALARM_ENABLED,
	ALARM_DISABLED	
} AlarmStatus_t;

typedef struct WatchTime_t
{
	char seconds;
	char minutes;
	char hours;
} WatchTime_t;

typedef struct TimeState_t {
	WatchTime_t tm;
	TimeUnitState_t st;
} TimeState_t;

typedef struct App_t {
	ModeState_t mode;
	StopwatchStatus_t swatchStatus;
	AlarmStatus_t alarmStatus;
	TimeUnitState_t timeUnit2Config;
	unsigned short tick;
} App_t;

// Variable Declarations
TimeState_t currentTime;

TimeState_t stopwatchTime;

TimeState_t alarmTime;
WatchTime_t alarmTime_conf;

WatchTime_t currentTime_conf;

// Function Declarations
void app_init(void);
void app(void);

#endif
