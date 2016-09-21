/* ex4.c - main, produce, consume */

#include <xinu.h>

process produce(void), consume(void);

int32 n = 0; /* Global variables are shared by all processes */ 


/*-----------------------------------------------------------------
 * main - Example of unsynchronized producer and consumer processes
 * ----------------------------------------------------------------
 */

process main(void)
{
  resume(create(consume, 1024, 20, "cons", 0));
  resume(create(produce, 1024, 20, "cons", 0));
  return OK;
}

/*-----------------------------------------------------------------
 * produce - Increment n 2000 times and exit
 * ----------------------------------------------------------------
 */

process produce(void) 
{
  int32 i;

  for(i=1; i<=2000; i++) {
    n++;
  }
  return OK;
}

/*-----------------------------------------------------------------
 * consume - Print n 2000 times and exit
 * ----------------------------------------------------------------
 */

process consume(void) 
{
  int32 i;

  for(i=1; i<=2000; i++) {
    printf("The value of n is %d \n", n);
	n--;
  }
  return OK;
}
