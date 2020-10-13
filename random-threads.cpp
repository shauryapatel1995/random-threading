#include <pthread.h>
#include <unordered_map>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>

std::unordered_map<int, pthread_t> threads; 
int id = 0;

/*
    CPU to schedule the thread on.
*/
int get_cpu_num() {
    return 1;
    // return rand() % 4; 
    
}

/*
    Just create a pthread here and assign it a processor.
*/
    int thread_create(void *(*start_routine)(void *), void *arg)
{
	srand(time(NULL));
    cpu_set_t mask; 
    CPU_ZERO(&mask);
    CPU_SET(get_cpu_num(), &mask);
    pthread_t thread;
    int a = pthread_create(&thread, NULL, start_routine, arg);
    
    if(a == 0) {
        pthread_setaffinity_np(thread, sizeof(mask), &mask);
        threads.insert(std::make_pair(id, thread));
    }

    return id++;
}

/*
    Call pthread.join
*/
void join_thread(int id) {
    // aa well
    if(threads.count(id) <= 0)
        return; 

    pthread_t t = threads.at(id);
    pthread_join(t, nullptr);
    threads.erase(id);
}

/*
    We don't need this anymore but well. 
*/
void join_all_threads() {
    // nope
}

// Self ID of thread. 
int getCurrentId() {
    return id - 1; 
}
