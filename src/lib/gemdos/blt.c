/*
   =============================================================================
	blt.c -- block image transfer functions for the Atari
	Version 1 -- 1988-08-15 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "osbind.h"

extern short	gl_hres;		/* horizontal resolution */

cpy_raster(src, dst, width )
register short	*src, *dst, width;
{
	if ((long)src > (long)dst) {

		while (width--)
			*dst++ = *src++;

	} else {

		while (width--)
			*(dst + width) = *(src + width);
	}
}

cpy_block(src_x, src_y, src_w, src_h, dst_x, dst_y )
short src_x, src_y;
register short src_w, src_h;
short dst_x, dst_y;
{
	register short	*pbase, *src_base, *dst_base;

	pbase = (short *)Physbase();

	if (src_y > dst_y) {

		(long)src_base = (long)pbase + (src_y * gl_hres) + src_x;
		(long)dst_base = (long)pbase + (dst_y * gl_hres) + dst_x;

		while (src_h--) {

			cpy_raster(src_base, dst_base, src_w);
			src_base += gl_hres;
			dst_base += gl_hres;
		}

	} else {

		(long)src_base = (long)pbase + ((src_y + src_h) * gl_hres);
		(long)dst_base = (long)pbase + ((dst_y + src_h) * gl_hres);

		while (src_h--) {

			cpy_raster(src_base, dst_base, src_w);
			src_base -= gl_hres;
			dst_base -= gl_hres;
		}
	}
}

