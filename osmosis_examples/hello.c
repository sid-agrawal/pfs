#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>

int main() {
   printf("Hello? Sleeping for 10 seconds...\n");
   sleep (10);

   struct timeval start;	/* starting time */
	struct timeval end;	/* ending time */
	unsigned long e_usec;	/* elapsed microseconds */

	gettimeofday(&start, 0);	/* mark the start time */
	sleep(2);		/* sleep for 2 seconds */
	gettimeofday(&end, 0);		/* mark the end time */

	/* now we can do the math. timeval has two elements: seconds and microseconds */
	e_usec = ((end.tv_sec * 1000000) + end.tv_usec) - ((start.tv_sec * 1000000) + start.tv_usec);

	printf("elapsed time: %lu microseconds\n", e_usec);
   
   printf("Hello? Sleeping forever\n");
   // Idle until killed
   while(1) {
      sleep(10);
   }

   return 0;
}
