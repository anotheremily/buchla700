/*
   =============================================================================
	egaes.c -- An example GEM application program in C
	Version 2 -- 1989-01-24 -- Metacomco / Lattice
   =============================================================================
*/

#include "portab.h"

/* a very useful macro */

#define   ADDR(a)   ((long)a)>>16,((long)a)&0xffff

char wind_name[] = "Example";

main ()
{
  WORD work_in[12], work_out[57];
  WORD handle, i, j, k, l, xstart, ystart, xwidth, ywidth, xcen, ycen;
  WORD gr_1, gr_2, gr_3, gr_4;
  WORD wi_1, wi_2, wi_3, wi_4;
  WORD w_handle;
  WORD clip[4];
  WORD mgbuf[8];
  WORD wind_type;
  WORD dflag, xt;

/* Set the system up to do GEM calls*/

  appl_init ();

/* Get the handle of the desktop */

  handle = graf_handle (&gr_1, &gr_2, &gr_3, &gr_4);

/* Open the workstation. */

  for (i = 0; i < 10; i++)
    work_in[i] = 1;
  work_in[10] = 2;

  v_opnvwk (work_in, &handle, work_out);

  graf_mouse (0, 1);
  wind_type = 0x002b;

/* 
*/

/* request size of desktop window */

  wind_get (0, 4, &wi_1, &wi_2, &wi_3, &wi_4);

/* calculate size of work area */

  wind_calc (1, wind_type, wi_1, wi_2, wi_3, wi_4, &i, &j, &k, &l);

/* make window of the max size */

  w_handle = wind_create (wind_type, wi_1, wi_2, wi_3, wi_4);
  wind_set (w_handle, 2, ADDR (wind_name), 0, 0);
  wind_open (w_handle, wi_1, wi_2, wi_3, wi_4);
  xstart = i;
  ystart = j;
  xwidth = k;
  ywidth = l;

  do
    {

      if (mgbuf[0] != 20)
	{			/* ignore redraw */

	  v_hide_c (handle);
	  clip[0] = xstart;
	  clip[1] = ystart;
	  clip[2] = xstart + xwidth - 1;
	  clip[3] = ystart + ywidth - 1;
	  vs_clip (handle, 1, clip);
	  xcen = xstart + xwidth / 2;
	  ycen = ystart + ywidth / 2;
	  vsf_interior (handle, 2);
	  vsf_style (handle, 8);
	  vsf_color (handle, 0);
	  v_bar (handle, clip);
	  vsf_interior (handle, 4);
	  vsf_color (handle, 1);
	  v_ellipse (handle, xcen, ycen, xwidth / 2, ywidth / 2);
	  v_show_c (handle);
	}

      evnt_mesag (&mgbuf);
/* 
*/
      if (mgbuf[0] == 28 || mgbuf[0] == 27)
	{

	  wind_calc (1, wind_type, mgbuf[4], mgbuf[5],
		     mgbuf[6], mgbuf[7], &xstart, &ystart, &xwidth, &ywidth);

	  if ((xt = xwidth) < gr_3)
	    {

	      xwidth = gr_3;
	      mgbuf[6] += gr_3 - xt;
	    }

	  if ((xt = ywidth) < gr_4)
	    {

	      ywidth = gr_4;
	      mgbuf[7] += gr_4 - xt;
	    }

	  dflag = 0;

	  wind_set (w_handle, 5, mgbuf[4], mgbuf[5], mgbuf[6], mgbuf[7]);
	}

    }
  while (mgbuf[0] != 22);

/* Close the workstation. */

  wind_close (w_handle);
  wind_delete (w_handle);
  v_clsvwk (handle);

/* Release GEM application */

  appl_exit ();

/* and quit */

  _exit (0);
}
