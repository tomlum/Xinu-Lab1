/* lab1app2a.c - lab1app */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lab1app2a  -  Process classification
 *------------------------------------------------------------------------
 */
process lab1app2b(void)
{

	struct procent *prptr;     /* table entry of current process */
	prptr = &proctab[currpid];

	kprintf("Doing work!\n");

	kprintf("Process category: %u\n", prptr->pr_class);
}
