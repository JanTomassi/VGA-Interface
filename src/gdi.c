/**
 * @file    gdi.c
 * @author  Jan Tomassi
 * @version V0.0.1
 * @date    02/10/2022
 * @brief   Graphics
 */

#include "stm32f4_discovery.h"

#include "gdi.h"
#include "video.h"
#include "string.h"
#include "font8x8.h"
#include "sys.h"

/**
 * @addtogroup VGA-Interface
 * @{
 * @addtogroup Graphics
 * @{
 */
#ifndef NULL
#define NULL 0
#endif

extern volatile u8 vsync;
extern u8 fb[VID_VSIZE][VID_HSIZE + 2];

const u8 gdiCloseBm[] = {0x7f, 0xC0,
                         0x7f, 0xC0,
                         0x7f, 0xC0,
                         0x7f, 0xC0,
                         0x40, 0x40,
                         0x7f, 0xC0,
                         0x7f, 0xC0,
                         0x7f, 0xC0,
                         0x7f, 0xC0};

/**
 * @brief Copy rectangle rc2 to rc1
 *
 * @param rc1 Destination rectangle
 * @param rc2 Source rectangle
 *
 * @return None
 */
void gdiCopyRect(PGDI_RECT rc1, PGDI_RECT rc2)
{

    rc1->x = rc2->x;
    rc1->y = rc2->y;
    rc1->w = rc2->w;
    rc1->h = rc2->h;
}

/**
 *
 *	@brief Bit Block Transfer funcion. This function uses the STM32 Bit-Banding mode
 *	to simplify the complex BitBlt functionality.
 *
 *	@note Cortex STM32F10x Reference Manual (RM0008):
 *	A mapping formula shows how to reference each word in the alias region to a
 *	corresponding bit in the bit-band region. The mapping formula is:
 *	bit_word_addr = bit_band_base + (byte_offset x 32) + (bit_number � 4)
 *	where:
 *	bit_word_addr is the address of the word in the alias memory region that
 *	maps to the targeted bit.
 *	bit_band_base is the starting address of the alias region
 *	byte_offset is the number of the byte in the bit-band region that contains
 *	the targeted bit bit_number is the bit position (0-7) of the targeted bit.
 *	Example:
 *	The following example shows how to map bit 2 of the byte located at SRAM
 *	address 0x20000300 in the alias region:
 *	0x22006008 = 0x22000000 + (0x300*32) + (2*4).
 *	Writing to address 0x22006008 has the same effect as a read-modify-write
 *	operation on bit 2 of the byte at SRAM address 0x20000300.
 *	Reading address 0x22006008 returns the value (0x01 or 0x00) of bit 2 of
 *	the byte at SRAM address 0x20000300 (0x01: bit set; 0x00: bit reset).
 *
 *	For further reference see the Cortex M3 Technical Reference Manual
 *
 *	@param prc		Clipping rectangle. All X/Y coordinates are inside "prc"
 *					If "prc" is NULL, the coordinates will be the entire display
 *					area
 *	@param	x			Bitmap X start position
 *	@param	y			Bitmap Y start position
 *	@param	w			Bitmap width, in pixels
 *	@param	y			Bitmap height, in pixels
 *	@param	bm			Pointer to te bitmap start position
 *	@param	rop			Raster operation. See GDI_ROP_xxx defines
 *
 *	@retval			none
 */
void gdiBitBlt(i16 x, i16 y, i16 w, i16 h, pu8 bm, u16 rop)
{

    u16 i, xz, xb, xt;
    u32 wb;         // Width in bytes
    u32 bit_number; // Start X position in bits (relative to x)
    u32 fb_offs_in_ram;
    u32 byte_number;
    u32 offs;
    u8 c;
    pu8 fbPtr; // Pointer to the Frame Buffer Bit-Band area
    pu8 fbBak;
    u8 fb1;
    u32 fb2;
    u32 rp;
    pu8 bmPtr; // Pointer to the bitmap bits

    //	Get total bitmap width in bytes
    wb = (u32)w >> 3;
    if ((wb << 3) < (u32)w)
        ++wb;
    xt = w;

    //	Get starting bit inside the first byte
    byte_number = (u32)x >> 3;
    bit_number = ((u32)x - (byte_number << 3));

    //	Draw bits
    for (i = 0; i < h; i++)
    {

        //	Clip Y
        if ((i + y) > (VID_VSIZE - 1))
            return;

        //	Get offset to frame buffer in bit-banding mode
        offs = (((u32)x >> 3)) + ((u32)(y + i) * VID_HSIZE_R);
        fb_offs_in_ram = (u32)(&fb - 0x20000000);
        fb_offs_in_ram += offs;
        fbPtr = (pu8)(0x22000000 + (fb_offs_in_ram * 32) + ((7 - bit_number) * 4));
        fbBak = (pu8)(0x22000000 + (fb_offs_in_ram * 32) + 28);

        //	Get offset to bitmap bits
        bmPtr = bm + ((u32)i * wb);
        xz = w;

        xb = 0;
        for (xz = 0; xz < xt; xz++)
        {
            fb1 = ((u32)fbPtr) & 0x000000E0;
            if (xb++ == 0)
            {
                c = *bmPtr;
                ++bmPtr;
            }
            xb &= 0x07;
            (c & 0x1) ? (rp = 1) : (rp = 0);

            while (!vsync)
                __WFI();
            switch (rop)
            {
            case GDI_ROP_COPY:
                *fbPtr = rp;
                break;
            case GDI_ROP_XOR:
                *fbPtr ^= rp;
                break;
            case GDI_ROP_AND:
                *fbPtr &= rp;
                break;
            case GDI_ROP_OR:
                *fbPtr |= rp;
                break;
            }
            fbPtr -= 4;
            fb2 = ((u32)fbPtr) & 0x000000E0;
            if (fb1 != fb2)
            {
                fbPtr = fbBak + 32;
                fbBak = fbPtr;
            }
            c >>= 1;
        }
    }
}

/**
 *	@brief a point in x/y position using the current graphical mode stored in
 *	grMode variable
 *
 *	@param x X position
 *	@param y Y position
 *	@param rop Raster operation. See GDI_ROP_xxx defines
 *
 *	@retval none
 */
void gdiPoint(PGDI_RECT rc, u16 x, u16 y, u16 rop)
{

    u16 w, r;
    u8 m;

    //	Test for point outside display area

    if (x >= VID_PIXELS_X || y >= VID_PIXELS_Y)
        return;

    w = x >> 3;
    r = x - (w << 3);

    //	Prepare mask

    m = (0x80 >> r);

    while (!vsync)
        __WFI();
    switch (rop)
    {
    case GDI_ROP_COPY:
        fb[y][w] |= m;
        break;
    case GDI_ROP_XOR:
        fb[y][w] ^= m;
        break;
    case GDI_ROP_AND:
        fb[y][w] &= m;
        break;
    }
}

/**
 *	@brief Draw line using Bresenham algorithm
 *
 *	@note This function was taken from the book:
 *	Interactive Computer Graphics, A top-down approach with OpenGL
 *	written by Emeritus Edward Angel
 *
 *	@param	prc			Clipping rectangle
 *	@param	x1			X start position
 *	@param	y1			Y start position
 *	@param	x2			X end position
 *	@param	y2			Y end position
 *	@param	rop			Raster operation. See GDI_ROP_xxx defines
 *
 *	@retval	none
 */
void gdiLine(PGDI_RECT prc, i16 x0, i16 y0, i16 x1, i16 y1, u16 rop)
{

    i16 dx, dy, i, e;
    i16 incx, incy, inc1, inc2;
    i16 x, y;

    dx = x1 - x0;
    dy = y1 - y0;

    if (dx < 0)
        dx = -dx;
    if (dy < 0)
        dy = -dy;
    incx = 1;
    if (x1 < x0)
        incx = -1;
    incy = 1;
    if (y1 < y0)
        incy = -1;
    x = x0;
    y = y0;

    if (dx > dy)
    {
        gdiPoint(prc, x, y, rop);
        e = 2 * dy - dx;
        inc1 = 2 * (dy - dx);
        inc2 = 2 * dy;
        for (i = 0; i < dx; i++)
        {
            if (e >= 0)
            {
                y += incy;
                e += inc1;
            }
            else
            {
                e += inc2;
            }
            x += incx;
            gdiPoint(prc, x, y, rop);
        }
    }
    else
    {
        gdiPoint(prc, x, y, rop);
        e = 2 * dx - dy;
        inc1 = 2 * (dx - dy);
        inc2 = 2 * dx;
        for (i = 0; i < dy; i++)
        {
            if (e >= 0)
            {
                x += incx;
                e += inc1;
            }
            else
            {
                e += inc2;
            }
            y += incy;
            gdiPoint(prc, x, y, rop);
        }
    }
}

/**
 *	@brief Draw rectangle
 *
 *	@param	x1			X start position
 *	@param	y1			Y start position
 *	@param	x2			X end position
 *	@param	y2			Y end position
 *	@param	rop			Raster operation. See GDI_ROP_xxx defines
 *
 *	@retval			none
 */
void gdiRectangle(i16 x0, i16 y0, i16 x1, i16 y1, u16 rop)
{

    gdiLine(NULL, x0, y0, x1, y0, rop);
    gdiLine(NULL, x0, y1, x1, y1, rop);
    gdiLine(NULL, x0, y0, x0, y1, rop);
    gdiLine(NULL, x1, y0, x1, y1, rop);
}

/**
 *	@brief Draw rectangle
 *
 *	@param	rc			Struct containing the rectangle parameters
 *	@param	rop			Raster operation. See GDI_ROP_xxx defines
 *
 *	@retval	none
 */
void gdiRectangleEx(PGDI_RECT rc, u16 rop)
{

    gdiRectangle(rc->x, rc->y, rc->x + rc->w, rc->y + rc->h, rop);
}

/**
 *	@brief Draw text inside rectangle
 *
 *	@param	prc			Pointer to clipping rectangle
 *	@param	ptext		Pointer to text
 *	@param	style		Text style (see GDI_WINCAPTION_xx defines)
 *	@param	rop			Raster operation. See GDI_ROP_xxx defines
 *
 *	@retval			none
 */
void gdiDrawText(PGDI_RECT prc, pu8 ptext, u16 style, u16 rop)
{

    u16 l, i, pos, xp;
    u8 c;
    pu8 ptx;

    l = strlen(ptext) * GDI_SYSFONT_WIDTH;
    switch (style)
    {
    case GDI_WINCAPTION_RIGHT:
        if (l < prc->w)
        {
            prc->x += (prc->w - l);
        }
        break;
    case GDI_WINCAPTION_CENTER:
        if (l < prc->w)
        {
            prc->x += ((prc->w - l) / 2);
        }
        break;
    }
    l = strlen(ptext);
    xp = 1; // prc->x;
    for (i = 0; i < l; i++)
    {
        c = *(ptext++);
        if (c >= GDI_SYSFONT_OFFSET)
        {
            pos = (u16)(c - GDI_SYSFONT_OFFSET) * GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
            ptx = ((pu8)gdiSystemFont) + pos;
            gdiBitBlt(xp, 0, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);
            xp += GDI_SYSFONT_WIDTH;
            if (xp >= ((prc->x + prc->w) - GDI_SYSFONT_WIDTH))
                return;
        }
    }
}

/**
 * @brief Draw text in X/Y position using system font.
 *
 * @param	x		X start position
 * @param	y		Y start position
 * @param	ptext	Pointer to text
 * @param	rop		Raster operation. See GDI_ROP_xxx defines
 * @param   alignment LEFT or RIGHT alignment. See GDI_ALIGNMENT
 *
 * @retval			none
 */
void gdiDrawTextEx(i16 x, i16 y, pu8 ptext, u16 rop, uint8_t alignment)
{
    u16 l, i, pos, xp;
    u8 c;
    pu8 ptx;

    l = strlen(ptext);
    if (alignment == GDI_LEFT_ALIGN)
        xp = x;
    else if (alignment == GDI_RIGHT_ALIGN)
        xp = VID_PIXELS_X - (x + (l * GDI_SYSFONT_WIDTH));
    else
        xp = x;

    for (i = 0; i < l; i++)
    {
        c = *(ptext++);
        if (c >= GDI_SYSFONT_OFFSET)
        {
            pos = (u16)(c)*GDI_SYSFONT_BYTEWIDTH * GDI_SYSFONT_HEIGHT;
            ptx = ((pu8)gdiSystemFont) + pos;
            gdiBitBlt(xp, y, GDI_SYSFONT_WIDTH, GDI_SYSFONT_HEIGHT, ptx, rop);

            if (alignment == GDI_LEFT_ALIGN)
                xp += GDI_SYSFONT_WIDTH;
            else if (alignment == GDI_RIGHT_ALIGN)
                xp += GDI_SYSFONT_WIDTH;

            if (xp >= VID_PIXELS_X)
                return;
        }
    }
}

void gdiInvertLine(u16 y)
{
    for (u16 x = 0; x < VID_HSIZE; x++)
    {
        while (!vsync)
            __WFI();
        fb[y][x] = ~fb[y][x];
    }
}

void gdiInvertTextLine(u16 y)
{
    for (u16 h = y; h < y + 8; h++)
    {
        for (u16 w = 0; w < VID_HSIZE; w++)
        {
            while (!vsync)
                __WFI();
            fb[h][w] = ~fb[h][w];
        }
    }
}

void gdiClearTextLine(u16 y)
{
    for (u16 h = y; h < y + 8; h++)
    {
        for (u16 w = 0; w < VID_HSIZE; w++)
        {
            while (!vsync)
                __WFI();
            fb[h][w] = 0;
        }
    }
}
///@}
///@}