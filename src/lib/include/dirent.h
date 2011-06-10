/*
   =============================================================================
	dirent.h -- GEMDOS directory entry structure
	Version 1 -- 1988-06-27 -- D.N. Lynx Crowe
   =============================================================================
*/

struct Dirent
{

  char name[8];
  char ext[3];
  char atrib;
  char res[10];
  char crtime[2];
  char crdate[2];
  char clus[2];
  char fsize[4];
};

/* attribute byte flags */

#define	F_RDONLY	0x01	/* Read-only file */
#define	F_HIDDEN	0x02	/* Hidden file */
#define	F_SYSTEM	0x04	/* System file */
#define	F_VOLUME	0x08	/* Volume label */
#define	F_SUBDIR	0x10	/* Sub-directory */
#define	F_ARCHIV	0x20	/* Archive flag */
