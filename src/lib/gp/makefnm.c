/*
   ============================================================================
	makefnm.c -- create a file name
	Version 1 -- 1988-07-13 -- D.N. Lynx Crowe
   ============================================================================
*/

extern char *rindex ();

/*
   ============================================================================
	makefnm(dest, src, ext) -- create a file name

		Where:

			char *dest	destination string pointer
			char *src	root name string pointer
			char *ext	extension string pointer  (inc. '.')
   ============================================================================
*/

makefnm (dest, src, ext)
     char *dest, *src, *ext;
{
  register char *cp;

  strcpy (dest, src);		/* copy the root name string */

  cp = rindex (dest, '.');	/* search for '.' */

  if (cp != (char *) 0L)	/* if it's got a '.' ... */
    strcpy (cp, ext);		/* ... copy ext string over the '.' */
  else
    strcat (dest, ext);		/* ... otherwise, append ext string */
}
