
#include "ee.h"

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4_discovery_lcd.h"
#include "lcd_log.h"

#include <stdio.h>

#include "Widget.h"
#include "WidgetConfig.h"
#include "Touch.h"
#include "STMPE811QTR.h"

#include "app.h"

/*
 * SysTick ISR2
 */
ISR2(systick_handler)
{
	/* count the interrupts, waking up expired alarms */
	CounterTick(myCounter);
}


TASK(TaskOUT)
{
	app();
}

TASK(TaskLCDTouch) {
	unsigned int px, py;
	TPoint p;
	if(GetTouch_SC_Async(&px, &py)) {
		p.x = px;
		p.y = py;
		OnTouch(weather_ui, &p);
	}

}

int main(void)
{
	/*
	 * Setup the microcontroller system.
	 * Initialize the Embedded Flash Interface, the PLL and update the
	 * SystemFrequency variable.
	 * For default settings look at:
	 * pkg/mcu/st_stm32_stm32f4xx/src/system_stm32f4xx.c
	 */
	SystemInit();

	/*Initialize Erika related stuffs*/
	EE_system_init();

	/*Initialize systick */
	EE_systick_set_period(MILLISECONDS_TO_TICKS(1, SystemCoreClock));
	EE_systick_enable_int();
	EE_systick_start();
	
	/* Init Touchscreen */
	IOE_Config();
	// InitTouch(-0.096, 0.0650, -367, 15);
	// InitTouch(-0.096, 0.0650, -327, 15);
	InitTouch(-0.1, 0.0650, -327, 15);


	/*Initialize the LCD*/
	STM32f4_Discovery_LCD_Init();
	LCD_Clear(APP_BACKGROUND_COLOR);
	LCD_SetColors(APP_BACKGROUND_COLOR, APP_BACKGROUND_COLOR);
	UI_DrawFixElements();
	// DrawInit(weather_ui);
	// DrawFixWidgets();
	LCD_SetFont(&Font8x12);

	app_init();

	/* Program cyclic alarms which will fire after an initial offset,
	 * and after that periodically
	 * */
	SetRelAlarm(AlarmOUT, 1, 10);
	SetRelAlarm(AlarmTaskLCDTouch, 7, 20);

	/* Forever loop: background activities (if any) should go here */
	for (;;);

}

