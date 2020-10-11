#include "threads.h"
#include <stdio.h>
#include <pthread.h>
#include <iostream>

void *PrintHello(void *threadid)
{
    long tid;
    tid = (long)threadid;
    cpu_set_t mask, mask1;
    CPU_ZERO(&mask1); 
    CPU_SET(1, &mask1);
    sched_getaffinity(0, sizeof(mask), &mask);
    std::cout << "Mask is " << CPU_EQUAL(&mask, &mask1) << std::endl;
    printf("Hello World! It's me, thread #%ld!\n", tid);
    pthread_exit(NULL);
}

int main() {   
    int i = 1;	
    int id = thread_create(PrintHello, (void *)&i);
    join_thread(id);
}
