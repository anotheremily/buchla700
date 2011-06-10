/*
   =============================================================================
	memset.c -- fill memory with short words
	Version 1 -- 1987-03-18 -- D.N. Lynx Crowe

	Set an array of n shorts starting at sp to the short w.
	Return pointer to sp.
   =============================================================================
*/

short *
memsetw (sp, w, n)
     register short *sp, w, n;
{
  register short *sp0 = sp;

  while (--n >= 0)
    *sp++ = w;

  return (sp0);
}
