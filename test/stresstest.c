#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "poolix.h"

pthread_mutex_t p_lock;

void bt_print(void* args);

int s_test_1(){
	printf("[+] Stress Test-1: START\n");
	// create pool then destroy pool
	struct threadpool_t *pool;
	int tadded = 0;

	if((pool = tpool_init(MAXTHREADS, MAXTASKS)) == NULL){
		printf("  [!] ERROR: POOL == NULL\n");
		return 1;
	}
	printf("  [*] Created threadpool_t with %d threads and queue size of %d\n", MAXTHREADS, MAXTASKS);

	printf("  [*] Adding %d tasks (bt_print) to pool in sparatic intervals\n", MAXTASKS*5);
	for (int i = 0; i < (MAXTASKS*5); i++){
		// sits in loop constatly trying to 
		// add task to queue 
		while(add_task(pool, &bt_print, NULL)){
			printf("[!] QUEUE FULL\n");
		}
		tadded++;
	}
	sleep(5);
	printf("  [*] Shutting down threadpool_t with %d threads and queue size of %d\n", MAXTHREADS, MAXTASKS);
	if(tpool_exit(pool)){
		printf("  [!] ERROR: FREE POOL FAILURE\n");
		return 1;
	}
	printf("Added %d tasks\n", tadded);
	printf("[+] Stress Test-1: SUCCESSFUL\n");
	return 0;
}



int main(){

	if(s_test_1()){
		printf("[!] Stress Test-1 failed\n");
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