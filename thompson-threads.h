#ifndef thompsonThreads
#define thompsonThreads
    /*
    *   Generate posteriors for thompson sampling.
    */
    void generate_posteriors(int groups);

    /*
    *   Update the posteriors.
    */
    void update_distributions(int);

    /*
    Function declaration for thread creation. 
    Eventually should replace pthread_create
    */
    int thread_create(void *(*start_routine)(void *), void *arg, int mode);

    void remove_distributions();

    int get_cpu(int id);

    int set_cpus(int cpus);

#endif