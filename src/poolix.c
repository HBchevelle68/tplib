#include <stdio.h>  	//std 
#include <stdlib.h>	    //malloc and other mem functions
#include <stdint.h> 	//uint16_t and other types
#include <pthread.h>	//pthreads

#include "poolix.h"	    //our bby


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
struct threadpool_t{

	pthread_t *t_pool;
	task_t *queue;
	pthread_mutex_t q_lock; // lock to access queue of tasks
    pthread_mutex_t s_lock; // lock to access status flags????????????
    uint8_t tp_status;
    uint8_t q_status;
    uint16_t ntask;
    uint16_t nopen; 
	uint16_t t_size;
	uint16_t q_size;
};

/*
    ALL threads will be constantly running this function
    They will loop and constantly check to see if a job
    is in Queue. If there is a job it will grab the job
    and go execute it.

    TO DO:
    -Need to make sure only 1 thread at a time can grab 
       a job
    -As soon as thread has needed info, needs to remove
       job from Queue, then allow other threads access 
       to job queue

*/

static void *thread_loop(void *threadpool){
    
    struct threadpool_t *tp = (struct threadpool_t *)threadpool;
    task_t to_execute;

    while(1) {

        pthread_mutex_lock(&(tp->q_lock));
        // Grab the function to execute
        if((tp->tp_status & SHUTDOWN) && tp->q_status == EMPTY){
            pthread_mutex_unlock(&(tp->q_lock));
            break;
        }
        else if(tp->q_status == EMPTY){
            pthread_mutex_unlock(&(tp->q_lock));
        }
        else{
            tp->tp_status |= RUNNING;
            to_execute.routine = tp->queue[tp->ntask].routine;
            to_execute.args = tp->queue[tp->ntask].args;

            // After task is pulled from queue
            // remove task from queue
            tp->queue[tp->ntask].routine = NULL;
            tp->queue[tp->ntask].args = NULL;

            // Are we at the end of the Queue?
            if(tp->ntask == (tp->q_size-1)){
                tp->ntask = 0;
                // Check if Queue is now empty
                if(tp->ntask == tp->nopen){
                    tp->q_status = EMPTY;
                }
            }// Increment and Check if Queue is now empty
            else if(++(tp->ntask) == tp->nopen){
                tp->q_status = EMPTY;
            }
            
            pthread_mutex_unlock(&(tp->q_lock));

            // Begin execution of function
            (*(to_execute.routine))(to_execute.args);
        }
    }

    pthread_exit(NULL);
    return NULL;
}


struct threadpool_t *tpool_init(int t_count, int q_size){
    // Threadpool pointer
    struct threadpool_t *tp;

    // Allocate threadpool_t struct
    if((tp = (struct threadpool_t *)malloc(sizeof(struct threadpool_t))) == NULL) {
        error = MALLOC;
        return NULL;
    }
    //Make sure values make sense
    if(t_count <= 0 || q_size <= 0 ) {
        error = MALLOC;
        return NULL;
    }
    else {
        tp->ntask = 0;
        tp->nopen = 0;
    	tp->t_size = t_count;
    	tp->q_size = q_size;
        tp->tp_status = NOTRUNNING; // set threadpool status flags
        tp->q_status = EMPTY; // set queue status flags
    }
    // Allocate threads memory
    if((tp->t_pool = (pthread_t *)malloc(sizeof(pthread_t) * t_count)) == NULL){
    	error = MALLOC;
        return NULL;
    }
    // Allocate tasks memory
    if((tp->queue = (task_t *)malloc(sizeof(task_t) * q_size)) == NULL){
    	error = MALLOC;
        return NULL;
    }
    for(int i = 0; i < q_size; i++){
        tp->queue[i].routine = NULL;
        tp->queue[i].args = NULL;

    }
    // pthread_mutext_init() returns 0 if successful
    if((pthread_mutex_init(&(tp->q_lock), NULL))){
        error = MLOCK;
        return NULL;
    }
    if((pthread_mutex_init(&(tp->s_lock), NULL))){
        error = MLOCK;
        return NULL;
    }
       
    /* Start threads */
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
    // Is there already a task here?

    if(tp->queue[tp->nopen].routine == NULL || tp->queue[tp->nopen].routine == NULL) {
        tp->queue[tp->nopen].routine = routine;
        tp->queue[tp->nopen].args = args;
        tp->q_status |= TODO;
        // At the end of queue?
        if(tp->nopen == (tp->q_size-1)){
            tp->nopen = 0; //set to beginning
        }
        else{
            tp->nopen++;
        }
        pthread_mutex_unlock(&(tp->q_lock));
    }
    else{
        pthread_mutex_unlock(&(tp->q_lock));
        error = QFULL; //set error flag
        return 1;
    }
    
    return 0;
}

/*
    @brief - Private function called by threadpool_exit()
             waits for threads to join and frees memory 
             and locks in proper order.

    @tp - Threadpool to teardown
*/
int free_pool(struct threadpool_t *tp){

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
    pthread_mutex_destroy(&(tp->s_lock));

    // Clean thread memory
    if(tp->t_pool != NULL){
        free(tp->t_pool);
    }

    // Clean queue memory
    if(tp->queue != NULL){
        free(tp->queue);
    }

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

