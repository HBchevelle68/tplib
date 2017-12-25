#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "tplib.h"


int main(){

  struct threadpool_t *workers;
  if((workers = tpool_init(MAXTHREADS)) == NULL){
    fprintf(stderr, "Error on tpool_init\n");
  }

  if(tpool_exit(workers)){
    fprintf(stderr, "Error on tpool_exit\n");
  }

  return 0;
}
