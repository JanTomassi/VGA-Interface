#ifndef __SYS_H
#define __SYS_H
/*
 *  stm32f4_discovery.h is located in Utilities/STM32F4-Discovery
 *  and defines the GPIO Pins where the leds are connected.
 *  Including this header also includes stm32f4xx.h and
 *  stm32f4xx_conf.h, which includes stm32f4xx_gpio.h
 */
#include "stm32f4_discovery.h"

#include "gdi.h"
#include <core_cm4.h>

// Public Var

//	Function definitions
void SystemInit(void); // borrowing the STM32F4 system init routines
u8 sysInitSystemTimer(void);
void sysTickDelay();
void sysTickDelayN(vu32 n);
void sysTickDelayS(vu32 n);

#endif // __SYS_H
