/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	int32  prio;
	qid16 readylist;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	prio = ptold->prprio;

	if (ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_CPUB) {
		
	} else if (ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_IOB) {
		
	} else if (ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_IOB) {
		
	} else if (ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_CPUB) {
		
	}

	readylist = prio != 500? readylists[prio+1] : readylists[0];
kprintf("1. I'm here!\n");
	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		//if (ptold->prprio > firstkey(readylist)) {
		//	return;
		//}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		ptold->pr_tsready = clktime;
		insert(currpid, readylist, ptold->prprio);
	}
kprintf("2. Now I'm here!\n");
	/* Force context switch to highest priority ready process */
	for(int i = 0; i < 9; i++){
		if(!isempty(readylists[i])){
			readylist = readylists[i];
			break;
		}
	}
	kprintf("3. and I made it!\n");
	currpid = dequeue(readylist);
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
kprintf("4.  Something else!");
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
