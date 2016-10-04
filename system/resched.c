/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	kprintf(".");
//kprintf("--Starting a Resched--------------------------------\n");
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	int32  prio;
	qid16 readylist;


	//kprintf("Okay, here's what the readylists look like\n");
	for(int i = 0; i < 10; i++){
		if(i==0){
			//kprintf("500 is %d\n", nonempty(readylists[i]));
		}
		else{
			//kprintf("%d is %d\n", i-1, nonempty(readylists[i]));
		}
	}

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {

	//kprintf("WOAH WE'RE DEFERRING OKAY THEN %d\n", currpid, prio);
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	prio = ptold->prprio;



	//kprintf("So the current pid is %d with priority %d\n", currpid, prio);


//kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
//kprintf("Old PR Class is %d and the old class was %d\n", ptold->pr_class, ptold->pr_prevclass);
if(prio != 500){
	if (prio > 0 && ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_IOB) {
			ptold->prprio--;
			//kprintf("But we're increasing the prio so it's now %d \n", prio);
	}
	else if (prio > 1 && ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_CPUB) {
			ptold->prprio-=2;
			//kprintf("But we're increasing the prio so it's now %d \n", prio);
		}
	else if (prio < 8 && ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_CPUB) {
			ptold->prprio++;
			//kprintf("But we're reducing the prio so it's now %d \n", prio);
		}  
	else if (prio < 7 && ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_IOB) {
			ptold->prprio+=2;
			//kprintf("But we're decreasing the prio so it's now %d \n", prio);
			
	} 
}
prio = ptold->prprio;
//kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");



	readylist = prio != 500? readylists[prio+1] : readylists[0];

	//kprintf("This process is of priority %d so let's insert it\n", prio);
	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		//Add code here to allow checking if the current one is the highest priority
		//if (ptold->prprio > firstkey(readylist)) {
		//	return;
		//}

		ptold->prstate = PR_READY;
		ptold->pr_tsready = clktime;
		insert(currpid, readylist, prio);
	}


	//kprintf("So now here's what the readylists looks like\n");
	for(int i = 0; i < 10; i++){
		if(i==0){
			//kprintf("500 is %d\n", nonempty(readylists[i]));
		}
		else{
			//kprintf("%d is %d\n", i-1, nonempty(readylists[i]));
		}
	}


	//kprintf("Now let's dequeue from the readylists\n");
	/* Force context switch to highest priority ready process */
	for(int i = 0; i < 10; i++){
		if(nonempty(readylists[i])){
			readylist = readylists[i];
			break;
		}
	}

	currpid = dequeue(readylist);
	//kprintf("and that pid is %d\n", currpid);
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;

	/* Reset quantum time for process */
	if (prio == 500) {
		preempt = 10;
	} else if (prio >= 0 && prio <= 2) {
		preempt = 200 - (prio * 20);
	} else if (prio >= 3 && prio <= 8) {
		preempt = 200 - (prio * 20) - 20;
	}

	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	//kprintf("--Ending a Resched--------------------------------\n");

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
