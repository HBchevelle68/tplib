#ifndef _tplib_H
#define _tplib_H

//Values just for dev purposes
#define TESTTHREADS 500

#ifndef FAILURE
  #define FAILURE 1
#endif



enum STATUS {
	NOTRUNNING = 0,
	RUNNING = 	 1,
	SHUTDOWN =   2,
	FORCE =      4
};

enum Q_STATUS{
	EMPTY = 	0,
	TODO = 		1,
	FULL = 	  2
};

enum ERROR{
	NONE = 	   0,
	MALLOC =   1,
	PTCREATE = 2,
	MLOCK =	   4,
	NOPOOL =   8,
	QFULL =   10
};


/* pool_attrib
	@brief - Sets GLOBAL attributes for pool(s)
	@return - Returns 0 if successful, 1 if errors

	@mtp - multi-pool support flag. set to 1 if mps is desired.
	       set to 0 if singualar pool

*/
int pool_attrib(int mps, int sighndl);

/* tpool_init
	@brief - Initializes a threadpool (struct threadpool_t) and
	         returns a pointer to a allocated threadpool with
	         t_count threads available and able to queue up
	         q_size tasks.
	@return - Returns a pointer to a threadpool if successful
	          and returns NULL if an error occured.

	@t_count - Thread count. Number of threads this pool will generate.
*/
struct threadpool_t *tpool_init(unsigned int t_count);

/* add_task
	@brief - Adds a task (function) to the threadpool's queue to
	         be exececuted.
	@return - Returns 0 if successful. Returns 1 if error occured
            and task is not added to Queue.

	@tp - Threadpool to add task to.
	@routine - Function to add to queue
	@args - Arguments needed for function. If no args, then pass NULL;
*/
int add_task(struct threadpool_t *tp, void(*routine)(void*), void *args);

/* tpool_exit
	@brief - Begins teardown of threadpool. Sets SHUTDOWN flag which
	         waits for all threads to join. Then deallocates all
	         memory.
	@return - Returns 0 if successful. Returns 1 if error occured
            and cannot guarantee memory was deallocated.

	@tp - Threadpool to teardown.
*/
int tpool_exit(struct threadpool_t *tp);



/*

            NEED TO IMPLEMENT

*/
void geterror();

#endif
