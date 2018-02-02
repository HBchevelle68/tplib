#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "bwpool.h"

#define RED           "\x1b[91m"
#define GREEN         "\x1b[92m"
#define YELLOW        "\x1b[93m"
#define COLOR_RESET   "\x1b[0m"

#define LOG_RED(X) printf("%s%s%s\n",RED, X, COLOR_RESET)
#define LOG_GREEN(X) printf("%s%s%s\n",GREEN, X, COLOR_RESET)
#define LOG_YELLOW(X) printf("%s%s%s\n",YELLOW, X, COLOR_RESET)

struct args{
  char placeholder;
};

pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
unsigned int counter;

static
int btest_1(){
  struct threadpool_t *workers;
  fprintf(stderr, "%s[*] Creating worker pool with %d threads %s\n",YELLOW, TESTTHREADS, COLOR_RESET);
  if((workers = tpool_init(TESTTHREADS)) == NULL){
    LOG_RED("[-] Error on tpool_init");
    return 1;
  }
  fprintf(stderr, "%s[*] Tearing down threadpool...%s\n",YELLOW, COLOR_RESET);
  if(tpool_exit(workers)){
    LOG_RED("[-] Error on tpool_exit");
    return 2;
  }
  return 0;
}

void testfunc(){
    pthread_mutex_lock(&print_lock);
    printf("Hello i am thread %d!!\n", ++counter);
    pthread_mutex_unlock(&print_lock);
}

static
int btest_2(){
    struct threadpool_t *workers;
    //struct args *a = NULL;
    fprintf(stderr, "%s[*] Creating worker pool with %d threads %s\n",YELLOW, TESTTHREADS, COLOR_RESET);
    if((workers = tpool_init(TESTTHREADS)) == NULL){
        LOG_RED("[-] Error on tpool_init");
        return 1;
    }

    fprintf(stderr, "%s[*] Adding 500 functions to work queue%s\n",YELLOW, COLOR_RESET);
    for(int i = 0; i < 500; i++){
        add_task(workers, testfunc, NULL);
    }
    sleep(2);
    fprintf(stderr, "%s[*] Tearing down threadpool...%s\n",YELLOW, COLOR_RESET);
    if(tpool_exit(workers)){
      LOG_RED("[-] Error on tpool_exit");
      return 2;
    }
    return 0;
}
int main(){
    int ret;

    #if (__SANITIZE_ADDRESS__)
         LOG_YELLOW("[*] Running basictest compiled with address sanitizer");
    #else
         LOG_YELLOW("[*] Running basictest compiled with NO address sanitizer");
    #endif
    if((ret = btest_1()) > 0){
      LOG_RED("[-] Basic Test 1 failure");
    }
    LOG_GREEN("[+] Basic Test 1 successful");

    if((ret = btest_2()) > 0){
      LOG_RED("[-] Basic Test 2 failure");
    }
    LOG_GREEN("[+] Basic Test 2 successful");
    return 0;
}
