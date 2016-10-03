/* ready.c - ready */

#include <xinu.h>

qid16	readylists[];			/* List of of ready lists		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;
	qid16 readylist;
	int32 prio;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	prptr->pr_tsready = clktime;
	prio = prptr->prprio;
	
	readylist = prio != 500? readylists[prio+1] : readylists[0];

	insert(pid, readylist, prio);
	resched();

	return OK;
}
