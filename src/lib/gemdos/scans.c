#include "osbind.h"
#include "stdio.h"
#include "stddefs.h"

#define	CON_DEV		2

main()
{
	int c;

	while ('\033' NE (0x00FF & (c = Bconin(CON_DEV)))) {

		printf("%08x\n", c);
	}
}
