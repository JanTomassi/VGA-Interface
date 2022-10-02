#include "sys.h"

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
inline void sysTickCount(void)
{
	uint32_t curTicks;

	curTicks = sysTicks; // Save Current SysTick Value
	while (sysTicks == curTicks)
	{
		__WFE(); // Power-Down until next Event/Interrupt
	}
}

/**
 * @brief Number of system clock tick to wait
 *
 */
void sysTickCountN(vu32 n)
{
	uint32_t curTicks;

	for (; n > 0; n--)
	{
		sysTickCount();
	}
}

/**
 * @brief Number of Second to wait
 *
 */
void sysTickCountS(vu32 n)
{
	uint32_t curTicks;
	n=n*1000;

	for (; n > 0; n--)
	{
		sysTickCount();
	}
}
