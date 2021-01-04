#ifndef randomThreads
#define randomThreads
// Expose the sThreads functions needed.

/*
    Function declaration for thread creation. 
    Eventually should replace pthread_create
*/
int thread_create(void *(*start_routine)(void *), void *arg, int mode);

/*
    Function to create threads for graph sampler based on threads types.
*/
int thread_create(void *(*start_routine)(void *), void *arg, int mode);
/*
    Join thread function
*/
void join_thread(int id);

/*
    Join All threads function
*/
void join_all_threads();

int getCurrentId();

int get_cpu(int id);

int set_cpus(int cpus);

void print_top_1(); 
void remove_distributions();
#endif
