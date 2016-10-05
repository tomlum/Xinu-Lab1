/* resched.c - resched, resched_cntl */

#include <xinu.h>
#define DBUG 0

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
																							if(DBUG){
																									kprintf(".");
																									kprintf("\n--Starting a Resched--------------------------------\n");
																							}
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	int32  prio;
	qid16 readylist;

																							if(DBUG){
																								kprintf("Okay, here's what the emptyness of the lists looks like\n");
																								for(int i = 0; i < 10; i++){
																									if(i==9){
																										kprintf("500 is %d\n", nonempty(readylists[i]));
																									}
																									else{
																										kprintf("%d is %d\n", i, nonempty(readylists[i]));
																									}
																								}
																							}

	/* If rescheduling is deferred, record attempt and return */
	if (Defer.ndefers > 0) {
																							if(DBUG){	kprintf("WOAH WE'RE DEFERRING OKAY THEN %d\n", currpid, prio);}
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */
	ptold = &proctab[currpid];
	prio = ptold->prprio;

																							if(DBUG){
																								kprintf("So the current pid is %d with priority %d\n", currpid, prio);
																								kprintf("~~Let's see if we need to reprioritize~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
																								kprintf("The proc's class is %d and its previous class was %d\n", ptold->pr_class, ptold->pr_prevclass);
																							}
	if(prio != 500 && preempt == QUANTUM){
		if (prio < 8 && ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_IOB) {
				ptold->prprio++;
																							if(DBUG){kprintf("So we're increasing the prio so it's now %d \n", ptold->prprio);}
		}
		else if (prio < 7 && ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_CPUB) {
				ptold->prprio+=2;
																							if(DBUG){kprintf("So we're increasing the prio so it's now %d \n", ptold->prprio);}
			}
		else if (prio > 0 && ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_CPUB) {
				ptold->prprio--;
																							if(DBUG){kprintf("So we're reducing the prio so it's now %d \n", ptold->prprio);}
			}  
		else if (prio > 1 && ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_IOB) {
				ptold->prprio-=2;
																							if(DBUG){kprintf("So we're decreasing the prio so it's now %d \n", ptold->prprio);}
				
		} 
	}
	else{
																							if(DBUG){kprintf("Nope, no need to reprioritize");}

	}
	prio = ptold->prprio;
																							if(DBUG){	kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");}
	
	/* Set readylist to the appropriate readylist */
	readylist = prio != 500? readylists[prio] : readylists[9];
	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		for(int i = 9; i >= 0; i--){
			if(nonempty(readylists[i])){
				if(prio > i){
					return; 
				}
				else{
					break;
				}
			}
		}

		ptold->prstate = PR_READY;
		ptold->pr_tsready = clktime;
		procPush(currpid, readylist, prio);
																							if(DBUG){	kprintf("This process is of priority %d so let's insert it\n", prio);}

	}
	else{
																									if(DBUG){	kprintf("But we shouldn't insert it to the readylist");}
	}



																							if(DBUG){	kprintf("So now here's what the emptyness of the readylists look like now\n");
																								for(int i = 0; i < 10; i++){
																										if(i==9){
																										kprintf("500 is %d\n", nonempty(readylists[i]));
																									}
																									else{
																										kprintf("%d is %d\n", i, nonempty(readylists[i]));
																									}
																								}


																								kprintf("Now let's dequeue from the readylists\n");
																							}
	/* Force context switch to highest priority ready process */
	for(int i = 9; i >= 0; i--){
		if(nonempty(readylists[i])){
			readylist = readylists[i];
																										if(DBUG){	kprintf("Readylist %d has an element!\n", i);}
			break;
		}
	}

	//If NULLPROC, don't bother context switching
	if(queuetab[queuehead(readylist)].qnext==NULLPROC && currpid==NULLPROC){
																							if(DBUG){		kprintf("All that's left is the NULLPROC!\n");
																							}
		return; //Stopping on Null Proc encounter doesn't work yet
	}

	currpid = dequeue(readylist);
																							if(DBUG){	kprintf("and that pid is %d\n", currpid);}
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
	

																							if(DBUG){	kprintf("--Ending a Resched--------------------------------\n");}

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
