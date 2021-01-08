#include "threads.h"
#include <unordered_map>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>
#include <pthread.h>

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
			degradation_graph.at(i).push_back(std::make_pair(thread_types.at(j), 0.0));
			trials.at(i).push_back(std::make_pair(thread_types.at(j), 0));
		}
	}
}

/*
*	Main Function to run the thread experiments and update the graph weights.
*/
void run_experiments()
{
	std::cout << "Running experiments: " << num_types << std::endl;

	// Create degradation graph and trial matrix.
	// create_graph();

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
				average_runtime = (average_runtime * (count) + (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()) / (count + 1));
				++count; 
			}
		}

		experiment_runtimes.insert(std::make_pair(thread_type, average_runtime));
	}

	for(auto it : experiment_runtimes) {
		std::cout << "Thread type: " << it.first << " average runtime: " << it.second << std::endl;
	}

	// Run experiments
	// Experiments flow.
	// We have threads of n types.
	// Select two types and select mode of running (same or different processor).
	// Update degradation based on mode.
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
