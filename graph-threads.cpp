#include "threads.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <cstdlib>
#include <algorithm>
#include <optional>

typedef std::chrono::steady_clock Clock;
using namespace std::chrono_literals;

// Store all threads by thread_type.
std::unordered_map<int, std::vector<std::pair<void *(*)(void *), void *>>> threads;

// Runtimes of each thread types.
std::unordered_map<int, double> experiment_runtimes;

// Degradation graph for threads.
std::unordered_map<int, std::unordered_map<int, double>> degradation_graph;

// num trials.
std::unordered_map<int, std::vector<std::pair<int, int>>> trials;

int num_types = 0;

std::vector<int> thread_types;

std::unordered_map<int ,std::vector<std::vector<int>>> sets;
std::vector<std::vector<int>> schedule_sets; 

// Create and store the threads based on their thread type.
int thread_create(void *(*start_routine)(void *), void *arg, int thread_type)
{
	// take the threads signature.
	// store it, in thread type, uniquely.
	if (threads.count(thread_type) > 0)
	{
		threads.at(thread_type).push_back(std::make_pair(start_routine, arg));
	}
	else
	{
		num_types++;
		std::cout << "Thread type is: " << thread_type << std::endl;
		threads.insert(std::make_pair(thread_type, std::vector<std::pair<void *(*)(void *), void *>>()));
		thread_types.push_back(thread_type);
		threads.at(thread_type).push_back(std::make_pair(start_routine, arg));
	}
	std::cout << "Finished creating thread" << std::endl;
	// return the internal thread id.
	return 0;
}

/*
	Creates a graph for the threads with the weights to be the degradations.
*/
void create_graph()
{
	for (int i = 0; i < thread_types.size(); i++)
	{
		degradation_graph.insert(std::make_pair(thread_types.at(i), std::unordered_map<int, double>()));
		trials.insert(std::make_pair(thread_types.at(i), std::vector<std::pair<int, int>>()));

		for (int j = 0; j < thread_types.size(); j++)
		{
			degradation_graph.at(thread_types.at(i)).insert(std::make_pair(thread_types.at(j), 0.0));
			trials.at(thread_types.at(i)).push_back(std::make_pair(thread_types.at(j), 0));
		}
	}
}

/*
	Function to check whether two vectors are the same.
*/
std::optional<std::vector<int> *> check_set_validity(std::vector<int> * threads, std::vector<int>* set_to_compare) {
	for(auto thread : *set_to_compare) {
		auto element = std::find(threads->begin(), threads->end(), thread);
		if(element != threads->end()) {
			threads->erase(element);
		} else {
			// not found return null.
			return std::nullopt;
		}
	}
	
	return threads;
}

// Used to construct the graph of execution.
// solver oriented operating systems. 
// ML and solvers have evolved.
void construct_sets()
{
	// Thread types to schedule.
	double time = 0;
	std::vector<int> threads = std::vector<int>();
	threads.push_back(1);
	threads.push_back(1);
	threads.push_back(2);
	threads.push_back(2);
	threads.push_back(2);
	threads.push_back(2);
	threads.push_back(1);
	threads.push_back(1);
	threads.push_back(1);
	threads.push_back(1);
	threads.push_back(1);
	threads.push_back(2);
	threads.push_back(2);
	threads.push_back(3);
	threads.push_back(3);
	threads.push_back(3);

	

	// At each step we might have a few options.
	// Either schedule with an existing set or schedule in a new set.
	// Level 0 of the sets are the threads itself.
	sets.insert(std::make_pair(1, std::vector<std::vector<int>>()));
	sets.at(1).push_back(std::vector<int>());
	sets.at(1).push_back(std::vector<int>());
	sets.at(1).push_back(std::vector<int>());
	sets.at(1).at(0).push_back(1);
	sets.at(1).at(1).push_back(2);
	sets.at(1).at(2).push_back(3);

	int num_levels = 16; // number of threads at max. 
	
	std::cout << "Starting graph creation" << std::endl;
	// For each level we need to figure something out.
	for(int i = 2; i <= num_levels; i++) {
		for(auto j : thread_types) {
			auto level_sets = sets.at(i - 1);
			for(auto thread_set : level_sets) {
				// For each set calculate if a new set should be added. 
				// Check degradation wrt each thread. Take current vector. Add current thread type to it.
				bool new_set = true; 
				for(auto t : thread_set) {
					// Check degradation wrt to each thread. 
					if(!(experiment_runtimes.at(t) + experiment_runtimes.at(j) > degradation_graph.at(t).at(j))) {
						new_set = false; 
						break;
					}
				}

				if(new_set) {
					std::vector<int> set_to_add = std::vector<int>(thread_set);
					set_to_add.push_back(j);
					if(sets.count(i) > 0) {
						sets.at(i).push_back(set_to_add);
					} else {
						sets.insert(std::make_pair(i, std::vector<std::vector<int>>()));
						sets.at(i).push_back(set_to_add);
					}
				} 
			}
		}
	}

	// for(int i = 1; i <= num_levels; i++) {
	// 	auto print_set = sets.at(i);
	// 	for(auto thread_set : print_set ) {
	// 		for(auto t : thread_set) {
	// 			std::cout << t << " ";
	// 		}
	// 		std::cout << ", ";
	// 	}
	// 	std::cout << std::endl;
	// }

	// Construct schedule sets from total sets.
	// Look at last level try to find set otherwise try to find nearest sets in previous level?
	// This can be done using LP.
	// This can also be done by creating a finite automata of sorts.

	
	// Let's do this naively for now. 
	// Just check each set for however much it matches. The set has to match perfectly.
	// This is just a basic way. 
	while (threads.size() > 0)
	{

		for (int i = num_levels; i > 0 && !threads.size() == 0; i--)
		{
			if (threads.size() < i)
			{
				continue;
			}

			std::cout << threads.size() << " " << i << std::endl;
			std::vector<std::vector<int>> sets_this_level;
			if (sets.count(i) > 0)
			{
				sets_this_level = sets.at(i);
			}
			else
			{
				continue;
			}

			

			for (auto level_set : sets_this_level)
			{
				auto new_threads = check_set_validity(new std::vector<int>(threads), &level_set);
				if (new_threads.has_value())
				{
					// update threads.
					threads.clear();
					threads.swap(*(*new_threads));
					schedule_sets.push_back(level_set);
				}

				if (threads.size() < i)
				{
					break;
				}
			}
		}
	}

	std::cout << "Generated sets: " << std::endl;

	for(int i = 0; i < schedule_sets.size(); i++) {
		auto current_set = schedule_sets.at(i);
		for(auto t : current_set) { 
			std::cout << t << " ";
		}
		std::cout << std::endl;
	}

}

void schedule_pthread() {
	int counter = 0; 
	auto t3 = Clock::now();
	// Schedule the constructed graph.
	std::vector<pthread_t> join;
	for (int i = 0; i < schedule_sets.size(); i++)
	{
		std::vector<int> threads_sched = schedule_sets.at(i);
		
		for (int j = 0; j < threads_sched.size(); j++)
		{
			++counter; 
			pthread_t pthread1;
			cpu_set_t mask1;
			auto thread_ops1 = threads.at(threads_sched.at(j));
			int thread1 = std::rand() % thread_ops1.size();

			// // Set CPU
			// CPU_ZERO(&mask1);
			// CPU_SET(0, &mask1);

			int a1 = pthread_create(&pthread1, NULL, thread_ops1.at(thread1).first, thread_ops1.at(thread1).second);

			// pthread_setaffinity_np(pthread1, sizeof(mask1), &mask1);

			join.push_back(pthread1);
		}

		
	}

	for (int k = 0; k < join.size(); k++)
	{
		pthread_t pthread = join.at(k);
		pthread_join(pthread, nullptr);
	}

	auto t4 = Clock::now();
	double experiment_time = std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count();
	std::cout << "Experiment two time: " << experiment_time << " Counter is: " << counter << std::endl;
}
/*
*	Schedule a thread using the generated graph weights and the threads scheduled so far. 
*	This will be maybe a greedy algorithm.
*   TODO - change this to use a Linear programming solver.
*/
void schedule()
{
	int counter = 0; 
	auto t1 = Clock::now();
	// Schedule the constructed graph.
	std::vector<pthread_t> join;
	for (int i = 0; i < schedule_sets.size(); i++)
	{
		std::vector<int> threads_sched = schedule_sets.at(i);

		for (int j = 0; j < threads_sched.size(); j++)
		{
			++counter; 
			pthread_t pthread1;
			cpu_set_t mask1;
			auto thread_ops1 = threads.at(threads_sched.at(j));
			int thread1 = std::rand() % thread_ops1.size();

			// Set CPU
			CPU_ZERO(&mask1);
			CPU_SET(0, &mask1);

			int a1 = pthread_create(&pthread1, NULL, thread_ops1.at(thread1).first, thread_ops1.at(thread1).second);

			pthread_setaffinity_np(pthread1, sizeof(mask1), &mask1);

			join.push_back(pthread1);
		}

		for (int k = 0; k < join.size(); k++)
		{
			pthread_t pthread = join.at(k);
			pthread_join(pthread, nullptr);
		}

		join.clear();
	}

	

	auto t2 = Clock::now();
	double experiment_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << "Experiment time: " << experiment_time<< " Counter is : " << counter << std::endl;
}

/*
*	Main Function to run the thread experiments and update the graph weights.
*/
void run_experiments()
{
	std::srand(std::time(nullptr)); // use current time as seed for random generator
	std::cout << "Running experiments: " << num_types << std::endl;

	// Create degradation graph and trial matrix.
	create_graph();

	// print the types and the pairs for now.
	for (auto it : threads)
	{
		int key = it.first;

		std::cout << key << " - " << std::endl;
		for (int j = 0; j < it.second.size(); j++)
		{
			std::cout << "Function: " << it.second.at(j).first << " args: " << it.second.at(j).second << std::endl;
		}
	}

	// First run all types in isolation 5 times to figure out the ground truth running time.
	// Save ground truth averages.
	// Run all thread types in isolation, but what should we categorize?
	// What if the same types of threads have very different running times? -- For now let's assume they have the same running time.
	for (auto it : threads)
	{
		int thread_type = it.first;
		int count = 0;
		double average_runtime = 0.0;
		while (count < 5)
		{

			for (int i = 0; i < it.second.size(); i++)
			{
				auto t1 = Clock::now();

				auto thread = std::thread(it.second.at(i).first, it.second.at(i).second);
				thread.join();

				auto t2 = Clock::now();
				std::cout << "Thread type: " << thread_type << " runtime: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;
				// update average time of thread_type and add to the map.
				average_runtime = ((average_runtime * (count)) + (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count())) / (count + 1);
				++count;
			}
		}
		
		experiment_runtimes.insert(std::make_pair(thread_type, average_runtime));
	}

	// Add the experiment times to calculate the degradation.
	for (auto it : experiment_runtimes)
	{
		std::cout << "Thread type: " << it.first << " average runtime: " << it.second << std::endl;
	}

	// Run experiments
	// Experiment setup
	cpu_set_t mask1, mask2;

	pthread_t pthread1, pthread2;
	int thread_type1 = 0, thread_type2 = 0;
	int thread1_counter = 0, thread2_counter = 0, experiment_count = 0;
	// Work done so far - Getting the ground truth per thread type.
	// Selecting two thread types per experiment randomly. Selecting threads for that experiment type randomly.
	// Running them on a random configuration of processors (same and diff).
	// Calculating the runtime.
	// Question - What if the thread types are same but runtimes are different?
	// One possible answer - For this case we can sample a subspace from the possible space of threads and then model individual degradations.
	// Why not use CPI instead of time, because its an independent measure of progress or progress points for tracking progress.
	// Question - How do we encode same vs different processors and how do we use these experiments to schedule.
	// Greedy set cover maybe.
	// Question - How do we define degradation, do we assign the degradation to the thread with the higher running time because the shorter thread will be hidden by the longer thread?
	// Likely answer - Degrdation happens to the longer thread for now because the shorter thread degradation doesn't really matter, but we can also keep global variables and modify them with the time?
	// Question - How do we stack the degradation - suppose that we have 5 threads of type 1, do we add degradation factor for all 5 threads?
	// Another good question but maybe can be answered at a later stage?
	// Oversubscription is a different problem, better to schedule A and B together then two A's together.
	// Self comparison is important. Have experiments to reflect possible number of threads.
	// Let's say we have a fixed number of threads and reflect all possible experiments or we have something like a thread pool and we only schedule k threads together then how do we schedule these together.
	// binpacking kind of thing, percentage of core usage. If it was perfect of two processes then 50% usage. But now they are 20% if they are bad.
	// maximize the amount of utilization you can get. latency vs throughput -> we need to be aware of the tradeoff!
	// Maybe look at total progress points per processor.
	// K threads on one processor -> how to schedule them!
	// Make the graph expectations and then verify the results.
	// The debut algorithm.
	// Run experiments 1-1 -> Don't choose at random for now, just run them sequentially each time.
	// The run experiments 2-1 (how to choose 2? We only choose threads which have very less degradation say 5%?)
	// We make a map of possible choices for the 1 -> {suitable candidates must not have a degradation of more than 5% with both}.
	// Same for 3 onwards.
	// Basically update the search space after each type of experiment is done.
	// Let's say the search space is of two threads then three threads etc.

	for (auto thread_type1 : thread_types)
	{
		for (auto thread_type2 : thread_types)
		{
			for (int i = 0; i < 5; i++)
			{

				// thread_type1 = thread_types.at(std::rand() % thread_types.size());
				// thread_type2 = thread_types.at(std::rand() % thread_types.size());

				// for now same processors.
				auto thread_ops1 = threads.at(thread_type1);
				int thread1 = std::rand() % thread_ops1.size();

				auto thread_ops2 = threads.at(thread_type2);
				int thread2 = std::rand() % thread_ops2.size();

				// Set CPU
				CPU_ZERO(&mask1);
				CPU_ZERO(&mask2);
				int cpu1 = 0, cpu2 = 0;
				// For now config is always 0.
				// int config = std::rand() % 2;
				int config = 0;

				if (config == 1)
					cpu2 = 1;

				CPU_SET(cpu1, &mask1);
				CPU_SET(cpu2, &mask2);

				auto t1 = Clock::now();

				int a1 = pthread_create(&pthread1, NULL, thread_ops1.at(thread1).first, thread_ops1.at(thread1).second);
				int a2 = pthread_create(&pthread2, NULL, thread_ops2.at(thread2).first, thread_ops2.at(thread2).second);
				pthread_setaffinity_np(pthread1, sizeof(mask1), &mask1);
				pthread_setaffinity_np(pthread2, sizeof(mask2), &mask2);

				pthread_join(pthread1, nullptr);
				pthread_join(pthread2, nullptr);

				auto t2 = Clock::now();
				double experiment_time = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
				std::cout << "Ran experiment: thread 1: " << thread_type1 << " thread2: " << thread_type2 << " CPU config: " << config << " runtime is: " << experiment_time << std::endl;

				// Update the degradation percentage and the trials.
				// std::this_thread::sleep_for(std::chrono::seconds(2));
				double current_time = degradation_graph.at(thread_type1).at(thread_type2);
				
				double average_runtime = ((current_time * (i)) + (experiment_time)) / (i + 1);
				
				degradation_graph.at(thread_type1)[thread_type2] =  average_runtime;
			}


		}
	}

	for(auto thread_type : degradation_graph) {
		auto thread_type1 = thread_type.first; 
		auto threads2 = thread_type.second;
		for(auto thread_type2 : threads2) {
			auto thread_name = thread_type2.first; 
			auto runtime = thread_type2.second; 

			std::cout << "Thread type 1: " << thread_type1 << " Thread type 2: " << thread_name << " Runtime: " << runtime << std::endl;
		}
	}

	construct_sets();

	// Maybe something about warmup of caches.
	schedule_pthread();

	std::this_thread::sleep_for(2s);

	schedule();

	std::this_thread::sleep_for(2s);

	schedule_pthread();
}



/*
*	Join thread based on ID.
*/
void join_thread(int thread_id)
{
}

void join_all_threads()
{
}

int getCurrentId()
{
}

void print_top_1()
{
}
