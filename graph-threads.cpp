#include "threads.h"
#include <unordered_map>
#include <vector>
#include <iostream>

// Store all threads by thread_type.
std::unordered_map<int, std::vector<std::pair<void *(*)(void *), void *>>> threads;

// Degradation graph for threads. 
std::vector< std::vector<int>> degradation_graph; 

int num_types = 0;

int thread_create(void *(*start_routine)(void *), void *arg, int mode) {

}

// Create and store the threads based on their thread type.
int thread_create(void *(*start_routine)(void *), void *arg, int thread_type) {
	// take the threads signature. 
	// store it, in thread type, uniquely. 
	if(threads.count(thread_type) > 0) {
		threads.at(thread_type).push_back(std::make_pair(start_routine, arg));
	} else {
		num_types++;
		threads.insert(std::make_pair(thread_type, std::vector<std::pair<void *(*)(void *), void *>>()));
		threads.at(thread_type).push_back(std::make_pair(start_routine, arg));
	}
	// return the internal thread id. 
	return 0;
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
	// print the types and the pairs for now. 
	for(int i = 1; i <= num_types; i++) {
		for(int j = 0; j < threads.at(i).size(); j++) {
			std::cout << "Function: " << threads.at(i).at(j).first << " args: " << threads.at(i).at(j).second << std::endl;
		}
	}

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


