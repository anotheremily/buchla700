/*
   =============================================================================
	bootsec.h -- {GEM|MS}DOS boot sector structure
	Version 1 -- 1988-01-27 -- D.N. Lynx Crowe
   =============================================================================
*/

struct BootSec
{

  char branch[2];
  char oem[6];
  char vsn[3];
  char bps[2];			/* LLHH */
  char spc;
  char res[2];			/* LLHH */
  char nfats;
  char ndirs[2];		/* LLHH */
  char nsects[2];		/* LLHH */
  char media;
  char spf[2];			/* LLHH */
  char spt[2];			/* LLHH */
  char nsides[2];		/* LLHH */
  char nhid[2];			/* LLHH */
  char boot[480];
  char cksum[2];		/* LLHH */
};
