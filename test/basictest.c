#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "poolix.h"

pthread_mutex_t p_lock;

void bt_print(void* args);

int test_1(){
	printf("[+] Test-1: START\n");
	// create pool then destroy pool
	struct threadpool_t *pool;

	if((pool = tpool_init(MAXTHREADS, MAXTASKS)) == NULL){
		printf("  [!] ERROR: POOL == NULL\n");
		return 1;
	}
	printf("  [*] Created threadpool_t with %d threads and queue size of %d\n", MAXTHREADS, MAXTASKS);

	if(tpool_exit(pool)){
		printf("  [!] ERROR: FREE POOL FAILURE\n");
		return 1;
	}
	printf("  [*] Shutting down threadpool_t with %d threads and queue size of %d\n", MAXTHREADS, MAXTASKS);
	printf("[+] Test-1: SUCCESSFUL\n");
	return 0;
}

int test_2(){
	printf("[+] Test-2: START\n");
	// create pool then destroy pool
	struct threadpool_t *pool;

	if((pool = tpool_init(MAXTHREADS, MAXTASKS)) == NULL){
		printf("  [!] ERROR: POOL == NULL\n");
		return 1;
	}
	printf("  [*] Created threadpool_t with %d threads and queue size of %d\n", MAXTHREADS, MAXTASKS);

	printf("  [*] Adding %d tasks (bt_print) to pool then exiting\n", MAXTHREADS);
	for (int i = 0; i < MAXTASKS; i++){
		add_task(pool, &bt_print, NULL);
	}
	sleep(5);
	if(tpool_exit(pool)){
		printf("  [!] ERROR: FREE POOL FAILURE\n");
		return 1;
	}
	printf("  [*] Shutting down threadpool_t with %d threads and queue size of %d\n", MAXTHREADS, MAXTASKS);
	printf("[+] Test-2: SUCCESSFUL\n");
	return 0;
}

int main(int argc, char* argv[]){
	
	if(test_1()){
		printf("[!] Test-1 failed\n");
		return 1;
	}	

	if(test_2()){
		printf("[!] Test-2 failed\n");
		return 1;
	}
	return 0;
}

void bt_print(void* args){
	pthread_mutex_lock(&p_lock);
	pthread_t id = pthread_self();
	printf("Thread: %zu\n", id);
	pthread_mutex_unlock(&p_lock);
}