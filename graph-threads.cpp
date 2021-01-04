#include "threads.h"
#include <iostream>

int thread_create(void *(*start_routine)(void *), void *arg, int mode) {

}

// Create and store the threads based on their thread type.
int thread_create(void *(*start_routine)(void *), void *arg, int mode, int thread_type) {
	std::cout << "Yes we got the thread" << std::endl;
}

void join_thread(int thread_id) {

}

void join_all_threads() {

}


int getCurrentId() {

}

int get_cpu(int id) {

}

int set_cpus(int cpus){

}

void print_top_1() {

}

void remove_distributions() {

}

void generate_posteriors(int id) {

}

void update_distributions(int id) {

} 


