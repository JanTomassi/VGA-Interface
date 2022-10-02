#include "demo.h"

const u8 deBadBoys1[] = {0x00, 0x00,
						 0x00, 0x00,
						 0x07, 0xc0,
						 0x1f, 0xf0,
						 0x3f, 0xf8,
						 0x33, 0x98,
						 0x3f, 0xf8,
						 0x0c, 0x60,
						 0x1b, 0xb0,
						 0x30, 0x18,
						 0x00, 0x00,
						 0x00, 0x00};

const u8 deBadBoys2[] = {0x00, 0x00,
						 0x00, 0x00,
						 0x07, 0xc0,
						 0x1f, 0xf0,
						 0x3f, 0xf8,
						 0x33, 0x98,
						 0x3f, 0xf8,
						 0x0c, 0x60,
						 0x1b, 0xb0,
						 0x0c, 0x60,
						 0x00, 0x00,
						 0x00, 0x00};

void demoInit(void)
{
	vidClearScreen();
	sysTickCountS(5);
	gdiRectangle(0, 0, (VID_PIXELS_X - 1), VID_VSIZE - 1, 0);
	gdiDrawTextEx(100, 40, (pu8) "VGA-INTERFACE", GDI_ROP_COPY);
	gdiDrawTextEx(100, 55, (pu8) "STM32F4-DISCOVERY", GDI_ROP_COPY);

	gdiDrawTextEx(100, 70, (pu8) "WWW.GITHUB.COM/JANTOMASSI", GDI_ROP_COPY);
	gdiDrawTextEx(100, 85, (pu8) "JAN TOMASSI", GDI_ROP_COPY);

	while (1)
	{
	};
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