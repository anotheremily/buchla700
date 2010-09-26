/*
   ===========================================================================
	LSEG.C -- Generalized line drawing function  (Integer DDA algorithm)
	Version 6 -- 1987-08-04 -- D.N. Lynx Crowe
	Copyright 1985, 1986, 1987 -- D.N. Lynx Crowe
   ===========================================================================
*/

short	(*point)();

#define	ABS(x)	((x) < 0 ? (-(x)) : (x))
#define	SIGN(x)	((x) < 0 ? (-1) : ((x) ? 1 : 0))

#define	LE	<=

/*
   =============================================================================
	lseg(x1, y1, x2, y2, t) -- draw a line from ('x1', 'y1') to ('x2', 'y2')
	in drawing mode 't'.

	All coordinates are short integers in whatever plotting units the
	'point' function expects.  No clipping or scaling is done, as this is
	the lowest level 'line' primitive in the drawing code, and coordinates
	are expected to have already been checked and found to be on screen. 

	The 'point' variable must be initialized to point at the
	pixel plotting function before this function is called.  The pixel
	plotting function is passed 3 arguments:  the x coordinate, the y
	coordinate, and the plotting mode.  It is declared here as type 'short',
	but any value it returns will be ignored.

	The register variable assignments were chosen based on the Alcyon C
	compiler for the Atari 1040ST and reflect the capabilities of that
	compiler.  Other compilers may require different optimizations based
	on their use of register variables.
   =============================================================================
*/

/* 
*/

lseg(x1, y1, x2, y2, t)
short x1, y1, x2, y2, t;
{
	register short dx, dy, ptx, pty, p;
	short i, px, py;

	p = x2 - (ptx = x1);
	dx = SIGN(p);
	py = ABS(p);

	p = y2 - (pty = y1);
	dy = SIGN(p);
	px = ABS(p);

	(*point)(ptx, pty, t);

	if (py > px) {

		p = py >> 1;

		for (i = 1; i < py; i++) {

			ptx += dx;

			if ( (p -= px) < 0) {

				pty += dy;
				p += py;
			}

			(*point)(ptx, pty, t);
		}

	} else {

		p = px >> 1;

		for (i = 1; i LE px; i++) {

			pty += dy;

			if ( (p -= py) < 0) {

				ptx += dx;
				p += px;
			}

			(*point)(ptx, pty, t);
		}
	}
}
