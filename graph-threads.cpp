#include "threads.h"
#include <iostream>

int thread_create(void *(*start_routine)(void *), void *arg, int mode) {

}

// Create and store the threads based on their thread type.
int thread_create(void *(*start_routine)(void *), void *arg, int mode, int thread_type) {
	// take the threads signature. 
	// store it, in thread type, uniquely. 
}

/*
	Creates a graph for the threads with the weights to be the degradations.
*/
void create_graph() {

}


/*
*	Main Function to run the thread experiments and update the graph weights.
*/
void run_experiments() {


}

/*
*	Schedule a thread using the generated graph weights and the threads scheduled so far. 
*	This will be maybe a greedy algorithm.
*/
void schedule() {

}

/*
*	Join thread based on ID.
*/
void join_thread(int thread_id) {

}

void join_all_threads() {

}


int getCurrentId() {

}

void print_top_1() {

}


