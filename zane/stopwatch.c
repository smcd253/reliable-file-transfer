#include <stdio.h>
#include <time.h>


int main()
{

  struct timespec start , stop;
  /*
   * defined in time.h 
   * sources: 
   * time.h defintions http://pubs.opengroup.org/onlinepubs/7908799/xsh/time.h.html 
   * GNU tutorial: https://www.gnu.org/software/libc/manual/html_node/Elapsed-Time.html
   * Stevens Unix Network Programming Volume 1 Chapter 6 
   */
 
  double total;

  clock_gettime(CLOCK_REALTIME , &start);

  clock_gettime(CLOCK_REALTIME, &stop);

  total = stop.tv_sec - start.tv_sec;
    
  printf("%f seconds\n", total);
 
  return 0;
}
