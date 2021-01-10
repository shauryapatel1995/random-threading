#include "threads.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <pthread.h>
#include <cstdlib>

typedef std::chrono::steady_clock Clock;

// Store all threads by thread_type.
std::unordered_map<int, std::vector<std::pair<void *(*)(void *), void *>>> threads;

// Runtimes of each thread types.
std::unordered_map<int, double> experiment_runtimes;

// Degradation graph for threads.
std::unordered_map<int, std::vector<std::pair<int, double>>> degradation_graph;

// num trials.
std::unordered_map<int, std::vector<std::pair<int, int>>> trials;

int num_types = 0;

std::vector<int> thread_types;

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
		degradation_graph.insert(std::make_pair(thread_types.at(i), std::vector<std::pair<int, double>>()));
		trials.insert(std::make_pair(thread_types.at(i), std::vector<std::pair<int, int>>()));

		for (int j = 0; j < thread_types.size(); j++)
		{
			degradation_graph.at(thread_types.at(i)).push_back(std::make_pair(thread_types.at(j), 0.0));
			trials.at(thread_types.at(i)).push_back(std::make_pair(thread_types.at(j), 0));
		}
	}
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

				// update average time of thread_type and add to the map.
				average_runtime = ((average_runtime * (count)) + (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count())) / (count + 1);
				++count; 
			}
		}

		experiment_runtimes.insert(std::make_pair(thread_type, average_runtime));
	}

	// Add the experiment times to calculate the degradation.
	for(auto it : experiment_runtimes) {
		std::cout << "Thread type: " << it.first << " average runtime: " << it.second << std::endl;
	}

	// Run experiments
	// Experiment setup
	cpu_set_t mask;
	CPU_ZERO(&mask);
	int cpu = 0;
	CPU_SET(cpu, &mask);
	pthread_t pthread1, pthread2;

	// Choose two random numbers. 
	while(true) {
		int thread_type1 = thread_types.at(std::rand() % thread_types.size());
		int thread_type2 = thread_types.at(std::rand() % thread_types.size());

		

		// for now same processors.
		auto thread_ops1 = threads.at(thread_type1);
		int thread1 = std::rand() % thread_ops1.size();

		auto thread_ops2 = threads.at(thread_type2);
		int thread2 = std::rand() % thread_ops2.size();

		auto t1 = Clock::now();

		int a1 = pthread_create(&pthread1, NULL, thread_ops1.at(thread1).first, thread_ops1.at(thread1).second);
		int a2 = pthread_create(&pthread2, NULL, thread_ops2.at(thread2).first, thread_ops2.at(thread2).second);
		pthread_setaffinity_np(pthread1, sizeof(mask), &mask);
		pthread_setaffinity_np(pthread2, sizeof(mask), &mask);

		pthread_join(pthread1, nullptr);
		pthread_join(pthread2, nullptr);
		
		auto t2 = Clock::now();
		double average_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
		std::cout << "Ran experiment: thread 1: " << thread_type1 << " thread2: " << thread_type2 << " runtime is: " << average_runtime  << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	

}

/*
*	Schedule a thread using the generated graph weights and the threads scheduled so far. 
*	This will be maybe a greedy algorithm.
*/
void schedule()
{
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
