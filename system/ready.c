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
kprintf("-----Starting a Ready--------------------------------\n");
	kprintf("Okay, here's what the readylists look like\n");
	for(int i = 0; i < 10; i++){
		if(i==0){
			kprintf("500 is %d\n", i, nonempty(readylists[i]));
		}
		else{
			kprintf("%d is %d\n", i-1, nonempty(readylists[i]));
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


	kprintf("So the current pid is %d\n", pid);
	kprintf("This process is of priority %d \n so let's insert it\n", prio);

	/* Set readylist to the appropriate readylist */

	if(0){
		prptr->prprio = 8;
		enqueue(pid, readylists[9]);
	}
	else{
		readylist = prio != 500? readylists[prio+1] : readylists[0];
		insert(pid, readylist, prio);
	}

	kprintf("So NOW the readylists look like\n");
	for(int i = 0; i < 10; i++){
		if(i==0){
			kprintf("500 is %d\n", i, nonempty(readylists[i]));
		}
		else{
			kprintf("%d is %d\n", i-1, nonempty(readylists[i]));
		}
	}
	

	kprintf("-----Ending a Ready--------------------------------\n");
	resched();

	return OK;
}
