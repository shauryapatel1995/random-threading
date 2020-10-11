#ifndef randomThreads
#define randomThreads
// Expose the sThreads functions needed.

/*
    Function declaration for thread creation. 
    Eventually should replace pthread_create
*/
int thread_create(void *(*start_routine)(void *), void *arg);

/*
    Join thread function
*/
void join_thread(int id);

/*
    Join All threads function
*/
void join_all_threads();

int getCurrentId(); 
#endif