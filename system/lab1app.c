/* lab1app.c - lab1app */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  lab1app1  -  prints the current value of pr_cputime and pr_tsready for the current
process, callsleepms() to sleep for three seconds, and then prints both values again
 *------------------------------------------------------------------------
 */
process lab1app1(void)
{
	intmask		mask;		/* Saved interrupt mask		*/
	int32		retval;		/* Value to return to caller	*/

	mask = disable();
	if (isbaddev(descrp)) {
		restore(mask);
		return SYSERR;
	}

	printf("%d\n", pr_cputime);
	printf("%d\n", pr_tsready);

	restore(mask);
	return retval;
}
