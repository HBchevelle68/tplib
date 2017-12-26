#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


#include "tplib.h"


int main(){
  printf("here1\n");
  struct threadpool_t *workers;
  if((workers = tpool_init(MAXTHREADS)) == NULL){
    fprintf(stderr, "Error on tpool_init\n");
  }
  printf("here2\n");
  //sleep(2);
  if(tpool_exit(workers)){
    fprintf(stderr, "Error on tpool_exit\n");
  }
  printf("here3\n");
  return 0;
}
