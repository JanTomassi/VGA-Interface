#ifndef	__VIDEO_H
#define	__VIDEO_H

//#include "gdptypes.h"		// removed, use types from stm32f4xx.h instead

#define	VID_HSIZE		100		// Horizontal resolution (in bytes)
//#define	VID_HSIZE		100		// Horizontal resolution (in bytes)
#define	VID_VSIZE		600		// Vertical resolution (in lines)

#define	VID_PIXELS_X	(VID_HSIZE * 8)
#define	VID_PIXELS_Y	VID_VSIZE
#define	VID_PIXELS_XR	(VID_PIXELS_X + 16)
#define	VID_HSIZE_R		(VID_HSIZE + 2)

//	Function definitions

void	vidInit(void);
void	vidClearScreen(void);
void TIM1_CC_IRQHandler(void) __attribute__((short_call()));

#endif	// __VIDEO_H