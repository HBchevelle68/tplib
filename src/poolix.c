#include <stdio.h>  	//std
#include <stdlib.h>	  //malloc and other mem functions
#include <stdint.h> 	//uint16_t and other types
#include <pthread.h>	//pthreads

#include "steque.h"
#include "poolix.h"


/*
    @error - Bit masked variable used to contained errors
             that may have occured. & compare with enum ERROR
             to find what error(s) may have occured.
*/
unsigned char error;


// Struct that holds the function and arguments to execute
typedef struct {
    void (*routine)(void *);
    void *args;
} task_t;

/*
    The core pool

    pthread_t *t_pool       - Thread array
    task_t *queue           - Queue holding tasks
    stats_t *stats          - NOT IMPLEMENTED YET
    pthread_mutex_t q_lock  - Mutex to access queue of tasks
    pthread_mutex_t s_lock  - Mutex to access status flags
    uint8_t tp_status       - Threadpool status flags
    uint8_t q_status        - Queue status flags
    uint16_t ntask          - Index of Next Task to execute
    uint16_t nopen          - Index of Next Open spot in Q
    uint16_t t_size         - Number of threads for pool
    uint16_t q_size         - Number of tasks for pool

*/
struct threadpool_t {
	pthread_t *t_pool;
  steque_t queue;
	pthread_mutex_t q_lock; // lock to access queue of tasks
  uint8_t tp_status;
  uint8_t q_status;
	uint16_t t_size;
	uint16_t q_size;
};

/*
    ALL threads will be constantly running this function
    They will loop and check to see if a job
    is in Queue. If there is a job it will grab the job
    and go execute it.

*/

static void *thread_loop(void *threadpool){

    struct threadpool_t *tp = (struct threadpool_t *)threadpool;
    task_t to_execute;

    while(1) {


        // Grab the function to execute
        if((tp->tp_status & SHUTDOWN) && tp->q_status == EMPTY){
            break;
        }
        pthread_mutex_lock(&(tp->q_lock));


        /*
          pop from queue
        */

        pthread_mutex_unlock(&(tp->q_lock));

        // Begin execution of function
        (*(to_execute.routine))(to_execute.args);

    }

    pthread_exit(NULL);
    return NULL;
}


struct threadpool_t *tpool_init(unsigned int t_count, unsigned int q_size){
    // Threadpool pointer
    struct threadpool_t *tp;

    // Allocate threadpool_t struct
    if((tp = malloc(sizeof(struct threadpool_t))) == NULL) {
        error = MALLOC;
        return NULL;
    }

  	tp->t_size = t_count;
  	tp->q_size = q_size;
    tp->tp_status = NOTRUNNING; // set threadpool status flags
    tp->q_status = EMPTY; // set queue status flags

    // INIT THREADS
    if((tp->t_pool = malloc(sizeof(pthread_t) * t_count)) == NULL){
    	  error = MALLOC;
        return NULL;
    }
    // INIT QUEUE
    steque_init(&(tp->queue));

    // INIT LOCKS
    pthread_mutex_init(&(tp->q_lock), NULL);
    //pthread_mutex_init(&(tp->s_lock), NULL);

    // Start threads
    for(int i = 0; i < t_count; i++) {
        if(pthread_create(&(tp->t_pool[i]), NULL, thread_loop, (void*)tp) != 0) {
            error = PTCREATE;
            return NULL;
        }
    }

    return tp;
}

int add_task(struct threadpool_t *tp, void (*routine)(void*), void *args){

    if(!tp){
        error = NOPOOL;
        return 1;
    }

    pthread_mutex_lock(&(tp->q_lock));



    return 0;
}

/*
    @brief - Private function called by threadpool_exit()
             waits for threads to join and frees memory
             and locks in proper order.

    @tp - Threadpool to teardown
*/
static int free_pool(struct threadpool_t *tp){

    // Make sure thread pool got allocated
    // Repeat process of checking for allocated memory
    // Free if memory allocated
    if(!tp){
        error = NOPOOL;
        return 1;
    }

    // Wait for threads to join
    for(int i = 0; i < tp->t_size; i++) {
        pthread_join(tp->t_pool[i], NULL);
    }

    // All threads are done, now safe to destroy locks`
    pthread_mutex_destroy(&(tp->q_lock));
    //pthread_mutex_destroy(&(tp->s_lock));

    // Clean thread memory
    if(tp->t_pool != NULL){
        free(tp->t_pool);
    }

    // Clean queue memory

    // Finally lets clean up threadpool memory
    free(tp);
    return 0;
}


int tpool_exit(struct threadpool_t *tp){

    tp->tp_status |= SHUTDOWN;
    if(free_pool(tp)){
        printf("ERROR in free_pool()\n");
        return 1;
    }
    return 0;
}