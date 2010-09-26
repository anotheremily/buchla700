#include "dos.h"
#include "errno.h"
#include "dir.h"

#define NULL (char *)0

static struct direct buffer;
extern int errno;

extern char *calloc(), *malloc(), *realloc();

scandir(dirname, namelist, select, compar)
char *dirname;
struct direct *(*namelist[]);
int (*select)();
int (*compar)();
{
	register struct direct **names;
	register int dirno;
	union REGS sregs, oregs;
	struct SREGS segregs;
	char *ptr, *paths;

	names = (struct direct **)calloc(1, sizeof(struct direct *));

	paths = calloc(128, 1);

	if(names == (struct direct **)0 || paths == NULL) {

		errno = ENOMEM;
		return(-1);
	}

	strcpy(paths, dirname);

	ptr = &paths[strlen(paths) - 1];

	if(*ptr == '/' || *ptr == '\\')
		*ptr = '\0';

	strcat(paths, "/*.*");

/* 
*/

	segread(&segregs);		/* set up segment registers */

	ptr = (char *)&buffer;

	sregs.h.ah = 0x1a;		/* set DTA to buffer */
	sregs.x.dx = FP_OFF(ptr);	/* offset */

#ifdef	M_I86LM
	segregs.ds = FP_SEG(ptr);	/* pointer in large model */
#endif	M_I86LM

	intdosx(&sregs, &oregs, &segregs);

	sregs.x.ax = 0x4e00;		/* search for first */
	sregs.x.cx = 0x1f;		/* include all attributes */
	sregs.x.dx = FP_OFF(paths);	/* offset to path */

#ifdef	M_I86LM
	segregs.ds = FP_SEG(paths);	/* segment for large model */
#endif	M_I86LM

	intdosx(&sregs, &oregs, &segregs);

	if(oregs.x.cflag) {

		errno = ENOTDIR;
		return(-1);
	}

	sregs.x.ax = 0x4f00;		/* search for next */

/* 
*/
	for(dirno = 0; oregs.x.cflag == 0; intdosx(&sregs, &oregs, &segregs)) {

		for(ptr = buffer.d_name; *ptr; ptr++)
			*ptr = tolower(*ptr);

		if(select == (int (*)())0 || (*select)(&buffer)) {

			names = (struct direct **)realloc((char *)names,
				(dirno + 2)*sizeof(struct direct *));

			if(names == (struct direct **)0) {

				errno = ENOMEM;
				return(-1);
			}

			names[dirno] = (struct direct *)calloc(1,
				sizeof(struct direct));

			if(names[dirno] == (struct direct *)0) {

				errno = ENOMEM;
				return(-1);
			}

			*names[dirno] = buffer;
			names[++dirno] = (struct direct *)0;
		}
	}

	if(compar != (int (*)())0)
		qsort((char *)names, dirno, sizeof(char *), compar);

	*namelist = names;
	free(paths);			/* free temp space */
	return(dirno);
}

/* 
*/

freedir(dirs)
register struct direct **dirs;
{
	register int ii;

	if(dirs == (struct direct **)0)
		return(-1);

	for(ii = 0; dirs[ii] != (struct direct *)0; ii++)
		free(dirs[ii]);

	free(dirs);
	return(0);
}

int
alphasort(dirptr1, dirptr2)
struct direct **dirptr1, **dirptr2;
{
	return(strcmp((*dirptr1)->d_name, (*dirptr2)->d_name));
}
