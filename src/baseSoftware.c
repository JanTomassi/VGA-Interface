#include "baseSoftware.h"
/**
 * @addtogroup VGA-Interface
 * @{
 * @addtogroup Example
 * @{
 */

__weak_symbol void programCallback()
{
	extern void Default_Handler();
	Default_Handler();
}

void initProgram(void)
{
	vidClearScreen();
	gdiRectangle(0, 0, (VID_PIXELS_X - 1), VID_VSIZE - 1, 0);
	sysTickDelayS(5);
	gdiDrawTextEx(100, 40, (pu8) "VGA-INTERFACE", GDI_ROP_COPY, GDI_LEFT_ALIGN);
	sysTickDelayS(1);
	gdiDrawTextEx(100, 55, (pu8) "STM32F4-DISCOVERY", GDI_ROP_COPY, GDI_LEFT_ALIGN);

	sysTickDelayS(1);
	gdiDrawTextEx(CHAR_ON_SCREEN_X(0), CHAR_ON_SCREEN_Y(74), (pu8) "www.github.com/JanTomassi", GDI_ROP_COPY, GDI_RIGHT_ALIGN);
	gdiDrawTextEx(CHAR_ON_SCREEN_X(0), CHAR_ON_SCREEN_Y(74), (pu8) "Jan Tomassi", GDI_ROP_COPY, GDI_LEFT_ALIGN);

	sysTickDelayS(5);
	programCallback();
}

/**
 * @brief K&R rand function
 *
 * @return pseudo-random integer on [0,32767]
 * @addtogroup Random Random number generator
 * @{
 */
static int next;

int rand(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed)
{
	next = seed;
}
/**@}*/
///@}
///@}