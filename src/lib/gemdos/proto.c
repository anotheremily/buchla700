/*
   =============================================================================
	proto.c -- dump Atari prototype boot sectors
	Version 2 -- 1988-01-25 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stdio.h"
#include "osbind.h"
#include "bootsec.h"

short	buf[256];	/* boot sector buffer */

char	*descrip[] = {	/* boot sector descriptions */

	"40 tracks, single sided (180K)",
	"40 tracks, double sided (360K)",
	"80 tracks, single sided (360K)",
	"80 tracks, double sided (720K)"
};

/* 
*/

/*
   =============================================================================
	getiwd() -- get an Intel byte reversed word
   =============================================================================
*/

unsigned
getiwd(wp)
register char wp[];
{
	register unsigned val;

	val = ((wp[1] & 0x00FF) << 8) | (wp[0] & 0x00FF);
	return(val);
}

/* 
*/

main()
{
	register long i;
	register short dt, j;
	register struct BootSec *bsp;

	bsp = (struct BootSwc *)buf;

	for (dt = 0; dt < 4; dt++) {

		memsetw(buf, 0xE5E5, 256);		/* 0xE5 fill */

		Protobt(buf, 0x01000000L, dt, 0);	/* make the prototype */

		printf("Atari prototype boot sector type %d:\n", dt);

		printf("     %s\n", descrip[dt]);

		printf("\nBoot sector contents --\n\n");

		printf("  Branch word       = $%02.2x $%02.2x\n",
			0x00FF & bsp->branch[0],
			0x00FF & bsp->branch[1]);

		printf("  OEM area          =");

		for (j = 0; j < 6; j++)
			printf(" $%02.2x", 0x00FF & bsp->oem[j]);

		printf("\n  Volume S/N        =");

		for (j = 0; j < 3; j++)
			printf(" $%02.2x", 0x00FF & bsp->vsn[j]);

		printf("\n  Bytes / Sector    = %u\n", getiwd(bsp->bps));
		printf("  Sectors / Cluster = %u\n", bsp->spc);
		printf("  Reserved sectors  = %u\n", getiwd(bsp->res));
		printf("  Number of FATS    = %u\n", bsp->nfats);
		printf("  Directory entries = %u\n", getiwd(bsp->ndirs));
		printf("  Total Sectors     = %u\n", getiwd(bsp->nsects));
		printf("  Media byte        = $%02.2x\n", 0x00FF & bsp->media);
		printf("  Sectors / FAT     = %u\n", getiwd(bsp->spf));
		printf("  Sectors / Track   = %u\n", getiwd(bsp->spt));
		printf("  Sides             = %u\n", getiwd(bsp->nsides));
		printf("  Hidden files      = %u\n", getiwd(bsp->nhid));
		printf("  Checksum          = $%04.4x\n\n", getiwd(bsp->cksum));

		printf("Dump of entire sector:\n");

		mdump((char *)&buf, (char *)&buf+511L, 0L);

		printf("\f");
	}

	exit(0);
}
