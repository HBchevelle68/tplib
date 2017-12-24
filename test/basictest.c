#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "tplib.h"


int test_1(){
	printf("[+] Test-1: START\n");
	// create pool then destroy pool
	struct threadpool_t *pool;

	if((pool = tpool_init(MAXTHREADS)) == NULL){
		printf("  [!] ERROR: POOL == NULL\n");
		return 1;
	}
	printf("  [*] Created threadpool_t with %d threads \n", MAXTHREADS);

	if(tpool_exit(pool)){
		printf("  [!] ERROR: FREE POOL FAILURE\n");
		return 1;
	}
	printf("  [*] Shutting down threadpool_t with %d threads d\n", MAXTHREADS);
	printf("[+] Test-1: SUCCESSFUL\n");
	return 0;
}

int main(int argc, char* argv[]){

	if(test_1()){
		printf("[!] Test-1 failed\n");
		return 1;
	}

	return 0;
}
