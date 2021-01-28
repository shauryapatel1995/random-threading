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
				double average_runtime = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
				std::cout << "Ran experiment: thread 1: " << thread_type1 << " thread2: " << thread_type2 << " CPU config: " << config << " runtime is: " << average_runtime << std::endl;

				// Update the degradation percentage and the trials.
				// std::this_thread::sleep_for(std::chrono::seconds(2));
			}
		}
	}

	// experiment 1
	auto t1 = Clock::now();
	thread_type1 = 2; 
	thread_type2 = 2;

	auto thread_exp1_ops1 = threads.at(thread_type1);
	int thread1 = std::rand() % thread_exp1_ops1.size();

	auto thread_exp1_ops2 = threads.at(thread_type2);
	int thread2 = std::rand() % thread_exp1_ops2.size();

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

	

	int a1 = pthread_create(&pthread1, NULL, thread_exp1_ops1.at(thread1).first, thread_exp1_ops1.at(thread1).second);
	pthread_setaffinity_np(pthread1, sizeof(mask1), &mask1);
	pthread_join(pthread1, nullptr);

	int a2 = pthread_create(&pthread2, NULL, thread_exp1_ops2.at(thread2).first, thread_exp1_ops2.at(thread2).second);
	pthread_setaffinity_np(pthread2, sizeof(mask2), &mask2);
	pthread_join(pthread2, nullptr);
	auto t2 = Clock::now();
	std::cout << "Experiment 1 witholding runtime: " << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count() << std::endl;

	// Experiment 2. 
	auto t3 = Clock::now();
	thread_type1 = 2; 
	thread_type2 = 2;
	auto thread_exp2_ops1 = threads.at(thread_type1);
	int thread3 = std::rand() % thread_exp2_ops1.size();

	auto thread_exp2_ops2 = threads.at(thread_type2);
	int thread4 = std::rand() % thread_exp2_ops2.size();

	// Set CPU
	CPU_ZERO(&mask1);
	CPU_ZERO(&mask2);
	
	CPU_SET(0, &mask1);
	CPU_SET(1, &mask2);

	

	int a3 = pthread_create(&pthread1, NULL, thread_exp2_ops1.at(thread3).first, thread_exp2_ops1.at(thread3).second);
	int a4 = pthread_create(&pthread2, NULL, thread_exp2_ops2.at(thread4).first, thread_exp2_ops2.at(thread4).second);
	pthread_setaffinity_np(pthread1, sizeof(mask1), &mask1);
	pthread_setaffinity_np(pthread2, sizeof(mask2), &mask2);

	pthread_join(pthread1, nullptr);
	pthread_join(pthread2, nullptr);

	
	auto t4 = Clock::now();
	std::cout << "Experiment 2 together runtime: " << std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count() << std::endl;

	// Experiment 3.
	auto t5 = Clock::now();
	thread_type1 = 2;
	thread_type2 = 2;
	auto thread_exp3_ops1 = threads.at(thread_type1);
	int thread5 = std::rand() % thread_exp3_ops1.size();

	auto thread_exp3_ops2 = threads.at(thread_type2);
	int thread6 = std::rand() % thread_exp3_ops2.size();

	

	int a5 = pthread_create(&pthread1, NULL, thread_exp3_ops1.at(thread5).first, thread_exp3_ops1.at(thread5).second);
	int a6 = pthread_create(&pthread2, NULL, thread_exp3_ops2.at(thread6).first, thread_exp2_ops2.at(thread6).second);
	
	pthread_join(pthread1, nullptr);
	pthread_join(pthread2, nullptr);

	auto t6 = Clock::now();
	std::cout << "Experiment 3 regular runtime: " << std::chrono::duration_cast<std::chrono::milliseconds>(t6 - t5).count() << std::endl;
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
