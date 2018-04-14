#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

typedef struct{

	uint8_t _10ms;
	uint8_t _100ms;
	uint8_t _1s;
} SysTimer_t;

extern SysTimer_t sysTimer;
extern SysTimer_t sysTimer_old;

void timer_init(void);

#endif