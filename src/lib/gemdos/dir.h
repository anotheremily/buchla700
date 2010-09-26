#ifndef	DIRSIZ
#define	DIRSIZ	13
#endif

/*
 *	Structure returned by search calls
 */
struct	direct
{
	char	rsvd[21];		/* reserved for dos */
	char	d_attr;			/* file attribute */
	long	d_time;			/* modified time */
	long	d_size;			/* file size */
	char	d_name[DIRSIZ];		/* directory entry name */
};

typedef struct _dirdesc
{
	int	d_magic;		/* magic number 1234 */
	int	d_length;		/* number of directory entries */
	int	d_pos;			/* current position */
	struct direct **namelist;	/* list of directory entries */
} DIR;

#define DMAGIC 0x1234

DIR *opendir();
struct direct *readdir();
long telldir();
