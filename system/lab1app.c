/* lab1app.c - lab1app */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lab1app1  -  Prints the current value of pr_cputime and pr_tsready for the 
 *  current process, callsleepms() to sleep for three seconds, and then prints 
 *  both values again.
 *------------------------------------------------------------------------
 */
 process lab1app(void)
{
	//intmask		mask;		/* Saved interrupt mask		*/

	//mask = disable();

	struct procent *prptr;     /* table entry of current process */
	prptr = &proctab[currpid];

	while(1) {
		kprintf("cputime: %u\n", prptr->pr_cputime);
		kprintf("tsready: %u\n", prptr->pr_tsready);
		sleepms(3000);
		kprintf("~~~~~~~~~~~~~~~~~~~~~\n");
	}

	//restore(mask);

	return OK;
}
