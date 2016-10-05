/* lab1app4tests.c - lab1app */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lab1app4tests  -  Process classification
 *------------------------------------------------------------------------
 */
process highPrioProc(void)
{
	struct procent *prptr;     /* table entry of current process */
	prptr = &proctab[currpid];

	sleepms(1);
	while(prptr->prprio!=8){	
		kprintf("\n): My Prio is only %d, it needs to be higher!!!\n", prptr->prprio);
	}
	kprintf("\n(: High Prio Proc is Happy!\n");
}

process lowPrioProc(void)
{
	struct procent *prptr;     /* table entry of current process */
	prptr = &proctab[currpid];

	while(prptr->prprio!=0){
		kprintf("\n:( My Prio is only %d, it needs to be lower!!!\n", prptr->prprio);
	}
	kprintf("\n:) Low Prio Proc is Happy!\n");
}