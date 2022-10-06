/**
 * @file    sys.c
 * @author  Jan Tomassi
 * @version V0.0.1
 * @date    02/10/2022
 * @brief   Systimer configuration
 */

#include "sys.h"
/**
 * @addtogroup VGA-Interface
 * @{
 * @addtogroup Systimer
 * @{
 */
volatile u32 sysTicks = 0;

/**
 * @brief Call every System Tick underflow
 *
 */
void SysTick_Handler(void)
{
	sysTicks++; // Increment Counter
}

/**
 * @brief set System Tick interval to 1ms
 *
 * @return u8 Success	1
 * 			  Fail		0
 */
u8 sysInitSystemTimer(void)
{

	if (SysTick_Config((150000) - 1))
	{
		return (0);
	}
	return (1);
}

/**
 * @brief one System clock tick
 *
 */
inline void sysTickDelay(void)
{
	uint32_t curTicks;

	curTicks = sysTicks; // Save Current SysTick Value
	while (sysTicks == curTicks)
	{
		__WFE(); 
	}
}

/**
 * @brief Number of system clock tick to wait
 *
 */
void sysTickDelayN(vu32 n)
{
	for (; n > 0; n--)
	{
		sysTickDelay();
	}
}

/**
 * @brief Number of Second to wait
 *
 */
void sysTickDelayS(vu32 n)
{
	n = n * 1000;

	for (; n > 0; n--)
	{
		sysTickDelay();
	}
}
///@}
///@}