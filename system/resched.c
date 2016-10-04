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
																								kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
																								kprintf("Old PR Class is %d and the old class was %d\n", ptold->pr_class, ptold->pr_prevclass);
																							}
	if(prio != 500 && preempt == QUANTUM){
		if (prio < 8 && ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_IOB) {
				ptold->prprio++;
																							if(DBUG){kprintf("But we're increasing the prio so it's now %d \n", prio);}
		}
		else if (prio < 7 && ptold->pr_class == PRCLS_IOB && ptold->pr_prevclass == PRCLS_CPUB) {
				ptold->prprio+=2;
																							if(DBUG){kprintf("But we're increasing the prio so it's now %d \n", prio);}
			}
		else if (prio > 0 && ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_CPUB) {
				ptold->prprio--;
																							if(DBUG){kprintf("But we're reducing the prio so it's now %d \n", prio);}
			}  
		else if (prio > 1 && ptold->pr_class == PRCLS_CPUB && ptold->pr_prevclass == PRCLS_IOB) {
				ptold->prprio-=2;
																							if(DBUG){kprintf("But we're decreasing the prio so it's now %d \n", prio);}
				
		} 
	}
	prio = ptold->prprio;
																							if(DBUG){	kprintf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");}
	
	/* Set readylist to the appropriate readylist */
	readylist = prio != 500? readylists[prio] : readylists[9];

																							if(DBUG){	kprintf("This process is of priority %d so let's insert it\n", prio);}

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
																							if(DBUG){		kprintf("it's been pushed\n");}
	}



																							if(DBUG){	kprintf("So now here's what the readylists looks like\n");
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
			break;
		}
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



	//If NULLPROC, don't bother context switching
	if(0 && currpid==NULLPROC){
																							if(DBUG){		kprintf("WOAH THAT'S THE NULL PROC OMG\n");
																									kprintf("I REPEAT THAT'S THE NULL PROC OMG\n");
																									kprintf("I'M STILL IN DISBELIEF THAT THAT'S THE NULL PROC OMG\n");
																							}
		return; //Stopping on Null Proc encounter doesn't work yet
	}
	else{
		ctxsw(&ptold->prstkptr, &ptnew->prstkptr);
	}

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
