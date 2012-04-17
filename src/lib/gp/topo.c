/*
   =============================================================================
	topo.c -- Topological Sort Algorithm
	Version 2 -- 1989-02-07 -- D.N. Lynx Crowe
   =============================================================================
*/

#include "stddefs.h"

/*
   =============================================================================

	Topological Sort Algorithm
	--------------------------

	Initialize an array T[i][j] such that
		if i is a predecessor of j, T[i][j] = 1,
		otherwise, T[i][j] = 0.

	Set the count of completed tasks to 0.

	Set the order[] array to zeros.

	Repeat:

		Search T for an unconstrained task k,
		such that T[i][k] = 0 for all i,
		and for that task:

			Set order[count] = k,		{ log task }

			Set T[k][j] = 0 for all j,	{ enable successors }

			Set T[k][k] = 1, and		{ mark task complete }

			Add 1 to the count of completed tasks.

	until no unconstrained tasks exist.

	If the number of completed tasks = the number of tasks, the job is done.
   =============================================================================
*/

/* 
*/

/*
   =============================================================================
	topo() -- Topological Sort Algorithm
   =============================================================================
*/

short
topo (n, T, order)
     register short n;		/* input: number of tasks */
     register char *T;		/* input: precedence array, char T[n][n] */
     register short order[];	/* output: task order array */
{
  register short count, j, k;

  count = 0;			/* initialize count of number of tasks done */

look:
  k = 0;

  do
    {

      /* look for an unconstrained task */

      for (j = 0; j < n; j++)
	if (T[(j * n) + k])	/* if any bit is set */
	  goto haspred;		/* ... it's constrained */

      order[count++] = k;	/* log the task */
      T[(k * n) + k] = 1;	/* mark the task complete */

      for (j = 0; j < n; j++)	/* trigger its successors */
	T[(k * n) + j] = 0;

      goto look;		/* look for another task */

    haspred:
    }
  while (++k <= n);

  if (count == n)		/* see if all are done */
    return (SUCCESS);
  else
    return (FAILURE);
}
