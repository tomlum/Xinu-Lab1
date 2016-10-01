/* starvation.c */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  starvation  -  Practicing starvation
 *------------------------------------------------------------------------
 */
process myapp(void)
{
  //intmask   mask;   /* Saved interrupt mask   */

  //mask = disable();

  struct procent *prptr;     /* table entry of current process */
  prptr = &proctab[currpid];

  while(1) {
    kprintf("I'm being inconsiderate to other processes\n");
    prptr->prprio = 1024;
  }

  //restore(mask);

  return OK;
}

process main(void)
{

  resume(create(myapp, 1024, 20, "test", 0));

  while(1) {
    kprintf("I am not starving!\r\n"); // hopefully we'll never see this
  }
  return OK;
}