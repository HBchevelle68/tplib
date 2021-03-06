#include <stdio.h>    //std
#include <stdlib.h>   //malloc and other mem functions
#include <stdint.h>   //uint16_t and other types
#include <pthread.h>  //pthreads
#include <signal.h>   //singal handler

#include "steque.h"
#include "bwpool.h"


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
    pthread_mutex_t q_lock  - Mutex to access queue of tasks
    pthread_mutex_t q_cond  - Condition variable for Queue
    uint8_t tp_status       - Threadpool status flags
    uint8_t q_status        - Queue status flags
    uint16_t t_size         - Number of threads for pool
    uint16_t q_size         - Number of tasks for pool

*/
struct threadpool_t {
  pthread_t *t_pool;
  steque_t queue;
  pthread_mutex_t q_lock; // lock to access queue of tasks
  pthread_cond_t q_cond; // condition variable for queue of tasks
  uint8_t tp_status;
  uint8_t q_status;
  uint16_t t_size;
};


static void safe_exit(struct threadpool_t *tp){
    pthread_mutex_unlock(&(tp->q_lock));
    pthread_exit(NULL);
}

/*
    ALL threads will be constantly running this function
    They will loop and check to see if a job
    is in Queue. If there is a job it will grab the job
    and execute.
*/
static void *thread_loop(void *threadpool){

    struct threadpool_t *tp = (struct threadpool_t *)threadpool;
    task_t *to_execute;

    while(1) {
        if((tp->tp_status & SHUTDOWN)){ break; }
        pthread_mutex_lock(&(tp->q_lock));
        while((tp->q_status = steque_isempty(&(tp->queue)))){
            if((tp->tp_status == GRACEFUL)){ safe_exit(tp); }
            pthread_cond_wait(&(tp->q_cond), &(tp->q_lock));
            if((tp->tp_status == SHUTDOWN)){ safe_exit(tp); }
        }
        //Pop task from Queue and release lock
        to_execute = steque_pop(&(tp->queue));
        pthread_mutex_unlock(&(tp->q_lock));

        // Begin execution of function
        (to_execute->routine)(to_execute->args);
        free(to_execute);
    }
    //Only gets here on 1st IF statement
    pthread_exit(NULL);
}


struct threadpool_t *tpool_init(unsigned int t_count){

    struct threadpool_t *tp;

    // Allocate threadpool_t struct
    if((tp = malloc(sizeof(struct threadpool_t))) == NULL) {
        error = MALLOC;
        return NULL;
    }

    tp->t_size = t_count;
    tp->tp_status = NOTRUNNING; //still not really used yet
    tp->q_status = EMPTY;

    // INIT THREADS
    if((tp->t_pool = malloc(sizeof(pthread_t) * t_count)) == NULL){
    	  error = MALLOC;
        return NULL;
    }
    // INIT QUEUE
    steque_init(&(tp->queue));

    // INIT LOCK/COND VAR
    pthread_mutex_init(&(tp->q_lock), NULL);
    pthread_cond_init(&(tp->q_cond), NULL);

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
    steque_t *item;
    task_t *task = malloc(sizeof(task_t));
    task->routine = routine;
    task->args = args;
    item = (void*)task;

    pthread_mutex_lock(&(tp->q_lock));
    steque_enqueue(&(tp->queue), item);
    tp->q_status = TODO;
    pthread_mutex_unlock(&(tp->q_lock));
    pthread_cond_broadcast(&(tp->q_cond));

    return 0;
}

/*
    @brief - Private function called by threadpool_exit()
             waits for threads to join and frees memory
             and locks in proper order.

    @tp - Threadpool to teardown
*/
static
int free_pool(struct threadpool_t *tp){

    // Make sure thread pool got allocated
    // Repeat process of checking for allocated memory
    // Free if memory allocated
    if(!tp){
        error = NOPOOL;
        return 1;
    }
    pthread_cond_broadcast(&(tp->q_cond));
    // Wait for threads to join
    for(int i = 0; i < tp->t_size; i++) {
        pthread_join(tp->t_pool[i], NULL);
    }

    // All threads are done, now safe to destroy locks`
    pthread_mutex_destroy(&(tp->q_lock));
    pthread_cond_destroy(&(tp->q_cond));


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

    tp->tp_status = SHUTDOWN;
    if(free_pool(tp)){
        printf("ERROR in free_pool()\n");
        return 1;
    }
    return 0;
}
