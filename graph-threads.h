#ifndef graphThreads
#define graphThreads
/*
    Function to create threads for graph sampler based on threads types.
*/
int thread_create(void *(*start_routine)(void *), void *arg, int thread_type);

/*
*   Run experiments to create the degradation graph.
*/
void run_experiments();

/*
*   Schedule threads with constructed degradation graph.
*/
void schedule(); 

void construct_graph();
#endif