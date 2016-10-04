/* ready.c - ready */

#include <xinu.h>
#define DBUG 0

qid16	readylists[];			/* List of of ready lists		*/

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
																									if(DBUG){ kprintf("\n-----Starting a Ready--------------------------------\n");
																										kprintf("Okay, here's what the readylists look like\n");
																										for(int i = 0; i < 10; i++){
																											if(i==9){
																												kprintf("500 is %d\n", nonempty(readylists[i]));
																											}
																											else{
																												kprintf("%d is %d\n", i, nonempty(readylists[i]));
																											}
																										}
																									}

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

																									if(DBUG){
																										kprintf("So the current pid is %d\n", pid);
																										kprintf("This process is of priority %d \n so let's insert it\n", prio);
																									}
	/* Set readylist to the appropriate readylist */

	if(pid==NULLPROC){
		prptr->prprio = 0;
		enqueue(NULLPROC, readylists[0]);
	}

	readylist = prio != 500? readylists[prio] : readylists[9];
	procPush(pid, readylist, prio);

																									if(DBUG){
																										kprintf("So NOW the readylists look like\n");
																										for(int i = 0; i < 10; i++){
																											if(i==9){
																												kprintf("500 is %d\n", nonempty(readylists[i]));
																											}
																											else{
																												kprintf("%d is %d\n", i, nonempty(readylists[i]));
																											}
																										}
																										

																										kprintf("-----Ending a Ready--------------------------------\n");
																									}
	resched();

	return OK;
}
