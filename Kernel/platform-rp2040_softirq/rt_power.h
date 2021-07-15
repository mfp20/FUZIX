#ifndef _SDK_POWER_DOT_H
#define _SDK_POWER_DOT_H

#include "rt.h"

typedef enum power_mode_e
{
	POWER_DOWN = 0,
	POWER_SAVE,
	POWER_STANDBY,
	POWER_STANDBY_EXTENDED,
	POWER_OFFSET, // 4
	POWER_LEVEL_USER,
	POWER_LEVEL_SAVE,
	POWER_LEVEL_BALANCED,
	POWER_LEVEL_PERFORMANCE,
	POWER_LEVEL_RAM, // 9
	POWER_DEFAULT = 255
} power_mode_t;

// power
void power_test_start(void);
void power_set_speed(power_mode_t i);
void power_set_mode(power_mode_t i);

#endif
