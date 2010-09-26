/*
   =============================================================================
	gplists.h -- general purpose list function header
	Version 2 -- 1988-07-15 -- D.N. Lynx Crowe
   =============================================================================
*/

struct list_e {			/* execution list entry */

	int	(*fn)();
	char	*arg;
};

struct list_1 {			/* singly linked list entry */

	struct	list1	*link1;
	int	(*fn)();
	char	*arg;
};

struct list_2 {			/* doubly linked list entry */

	struct	list2	*link1;
	struct	list2	*link2;
	int	(*fn)();
	char	*arg;
};

