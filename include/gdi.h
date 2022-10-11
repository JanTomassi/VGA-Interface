#ifndef __GDI_H
#define __GDI_H

#include "stm32f4_discovery.h"

typedef s32 i32;
typedef s16 i16;
typedef s8 i8;

typedef u32 *pu32;
typedef i32 *pi32;
typedef u16 *pu16;
typedef i16 *pi16;
typedef u8 *pu8;
typedef i8 *pi8;

//	System font

#define GDI_SYSFONT_WIDTH 8		// Width in pixels
#define GDI_SYSFONT_HEIGHT 8	// Height in pixels
#define GDI_SYSFONT_BYTEWIDTH 1 // Width in bytes
#define GDI_SYSFONT_OFFSET 0x20

//	RASTER OPERATION

#define GDI_ROP_COPY 0
#define GDI_ROP_XOR 1
#define GDI_ROP_AND 2
#define GDI_ROP_OR 3

typedef struct
{
	i16 x; // X position
	i16 y; // Y position
	i16 w; // Width
	i16 h; // Height

} GDI_RECT, *PGDI_RECT;

#define GDI_WINCAPTION 0x0001
#define GDI_WINBORDER 0x0002
#define GDI_WINCLOSEICON 0x0003

//	Text align mode

#define GDI_WINCAPTION_LEFT 0x0000
#define GDI_WINCAPTION_CENTER 0x0010
#define GDI_WINCAPTION_RIGHT 0x0020
#define GDI_WINCAPTION_MASK 0x0030

typedef struct
{
	u16 style;	 // Mode, see GDI_WINxxx defines
	GDI_RECT rc; // Absolute rectangle
	pu8 caption; // Caption text

} GDI_WINDOW, *PGDI_WINDOW;

typedef struct
{
	i16 w;	 // Width in bits
	i16 h;	 // Height in bits
	i16 wb;	 // width in bytes
	i16 wh;	 // Height in bytes
	pu8 *bm; // Pointer to bitmap bits

} GDI_BITMAP, PGDI_BITMAP;

#define CHAR_ON_SCREEN_X(x) (x << 3) + 1
#define CHAR_ON_SCREEN_Y(y) (y << 3)

typedef enum alignment
{
	GDI_LEFT_ALIGN,
	GDI_RIGHT_ALIGN
} GDI_ALIGNMENT;

//	Function definitions
void gdiGetClientRect(PGDI_WINDOW, PGDI_RECT);
void gdiCopyRect(PGDI_RECT rc1, PGDI_RECT rc2);
void gdiBitBlt(i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop);
void gdiPoint(PGDI_RECT rc, u16 x, u16 y, u16 rop);
void gdiLine(PGDI_RECT prc, i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void gdiRectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop);
void gdiRectangleEx(PGDI_RECT rc, u16 rop);
void gdiCircle(u16 x, u16 y, u16 r, u16 rop);
void gdiDrawTextEx(i16 x, i16 y, char* ptext, u16 rop, uint8_t alignment);
void gdiInvertLine(u16 y);
void gdiInvertTextLine(u16 y);
void gdiClearTextLine(u16 y);

#endif // __GDI_H
