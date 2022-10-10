#ifndef __DEMO_H
#define __DEMO_H

/*
 *  stm32f4_discovery.h is located in Utilities/STM32F4-Discovery
 *  and defines the GPIO Pins where the leds are connected.
 *  Including this header also includes stm32f4xx.h and
 *  stm32f4xx_conf.h, which includes stm32f4xx_gpio.h
 */
#include "stm32f4_discovery.h"

#include "sys.h"
#include "video.h"
#include "gdi.h"
#include "string.h"
#include <stdlib.h>

void initProgram(void);
void programCallback(void);

#endif