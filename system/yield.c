/* yield.c - yield */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  yield  -  Voluntarily relinquish the CPU (end a timeslice)
 *------------------------------------------------------------------------
 */
syscall	yield(void)
{
	intmask	mask;			/* Saved interrupt mask		*/

	mask = disable();

	struct procent *prptr;     /* table entry of current process */
	prptr = &proctab[currpid];

	prptr->pr_prevclass = prptr->pr_class; /* Save previous class state */
	prptr->pr_class = PRCLS_IOB; /* Process is I/O-bound */

	resched();
	restore(mask);
	return OK;
}
