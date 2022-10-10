#ifndef __SYS_H
#define __SYS_H

#include "stm32f4_discovery.h"

#include <core_cm4.h>
#include "gdi.h"
#include "scheduler.h"

void SystemInit(void); // borrowing the STM32F4 system init routines
u8 sysInitSystemTimer(void);
void sysTickDelay();
void sysTickDelayN(vu32 n);
void sysTickDelayS(vu32 n);

#endif // __SYS_H
