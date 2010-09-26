#include "stdio.h"
#include "dir.h"

main(argc, argv)
int argc;
char *argv[];
{
	DIR *dirptr;
	struct direct *ent;
	char *ptr, *strrchr();

	if(argc < 2) {

		argv[1] = ".";

	} else if((ptr = strrchr(argv[1], '/')) == NULL) {

		argv[1] = ".";

	} else if(ptr != argv[1]) {

		*ptr = '\0';
	}

	if((dirptr = opendir(argv[1])) == (DIR *)0) {

		fprintf(stderr, "dirtest1: ");
		perror(argv[1]);
		exit(1);
	}

	while((ent = readdir(dirptr)) != (struct direct *)0) {

		printf("%s, size = %ld\n",
		   ent->d_name, ent->d_size);
	}

	closedir(dirptr);
}
