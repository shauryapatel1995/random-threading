#include <pthread.h>
#include <unordered_map>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <random>
#include <unordered_set>

std::unordered_map<int, pthread_t> threads;
std::unordered_map<int, int> cpus;
std::unordered_map<int, std::normal_distribution<double>> posteriors;
std::unordered_map<int, int> num_trials;
std::unordered_set<int> fingerprints;
std::geometric_distribution<int> d;
int id = 0;
int sample = 0;
/*
    Needs to perform Thompson sampling.
    Lets focus on how to implement that. 
*/

std::default_random_engine generator;
std::normal_distribution<double> same(1, 1);
std::normal_distribution<double> different(1, 1);
int curr_cpu = 0;
int curr_mode = 1;
int the_mode = 0;
int num_trials_same = 0;
int num_trials_diff = 0;
double running_best = 1000.0;
/*
	Check fingerprints of current schedule 
*/
bool check_fingerprint(int s)
{
	std::cout << "Generating fingerprint: " << s << std::endl;
	std::unordered_map<int, int> map;
	int curr = 0;
	int fingerprint = 0;
	while (s > 0)
	{
		int a = s % 10;
		s = s / 10;
		if (map.count(a) > 0)
		{
			fingerprint = fingerprint * 10 + map.at(a);
		}
		else
		{
			map.insert(std::make_pair(a, curr));
			curr++;
			fingerprint = fingerprint * 10 + map.at(a);
		}
	}

	std::cout << "Checking fingerprint: " << fingerprint << std::endl;
	if (fingerprints.count(fingerprint) > 0)
		return false;

	fingerprints.insert(fingerprint);
	return true;
}

/*
Helper function to generate all trials
*/
void generate_trials(int curr, int *possibles, int groups)
{
	std::cout << curr << std::endl;
	if (curr > 9999999 && curr < 100000000)
	{
		if (check_fingerprint(curr))
		{
			posteriors.insert(std::make_pair(curr, std::normal_distribution<double>(1, 1)));
			num_trials.insert(std::make_pair(curr, 0));
			curr_mode = curr;
			the_mode = curr;
		}
		return;
	}

	for (int i = 0; i < groups; i++)
	{
		generate_trials(curr * 10 + possibles[i], possibles, groups);
	}

	sample = d(generator);
	return;
}

/*
Generate posterior for all possible actions preregister those posteriors
*/
void generate_posteriors(int groups)
{
	std::cout << "Generating posteriors " << groups << std::endl;
	int *possibles = malloc(sizeof(int) * groups);
	for (int i = 1; i <= groups; i++)
	{
		possibles[i] = i;
	}

	generate_trials(1, possibles, groups);
}

/*
    CPU to schedule the thread on.
*/
int get_cpu_num()
{
	// return 0;
	// unsigned int a = static_cast<unsigned int>(time(NULL) + id);
	// return rand_r(&a) % 1;
	// Here instead of random we sample from posteriors.
	/*if(curr_mode == 0) {
	// same cpu 
	return curr_cpu;
    } else {
	// Different cpus. 
	curr_cpu++;
	return curr_cpu;
    }*/
	int cpu = curr_mode % 10;
	curr_mode /= 10;
	return cpu;
}

int set_cpus(int cpus)
{
	curr_mode = cpus;
}

/*
    Just create a pthread here and assign it a processor.
*/
int thread_create(void *(*start_routine)(void *), void *arg, int mode)
{
	// srand(time(NULL));
	cpu_set_t mask;
	CPU_ZERO(&mask);
	int cpu = get_cpu_num();
	std::cout << "CPU is: " << cpu << std::endl;
	CPU_SET(cpu, &mask);
	pthread_t thread;
	int a = pthread_create(&thread, NULL, start_routine, arg);

	if (mode == 1)
		pthread_setaffinity_np(thread, sizeof(mask), &mask);
	threads.insert(std::make_pair(id, thread));
	cpus.insert(std::make_pair(thread, cpu));

	return id++;
}

/*
    Call pthread.join
*/
void join_thread(int id)
{
	// aa well
	if (threads.count(id) <= 0)
		return;

	pthread_t t = threads.at(id);
	pthread_join(t, nullptr);
	threads.erase(id);
}

int get_cpu(int id)
{
	return cpus.at(id);
}

/*
    We don't need this anymore but well. 
*/
void join_all_threads()
{
	// nope
}

// Self ID of thread.
int getCurrentId()
{
	return id - 1;
}

/*void update_distributions(int total_time) {
	std::cout << "Current number of trials: " << num_trials_same << " " << num_trials_diff << std::endl;
	std::cout << "Current distribution means: " << same.mean() << " " << different.mean() << std::endl;
	if(curr_mode == 0) {
		num_trials_same++;
		// Update first.
		double mean = same.mean();
		mean = (double)(mean*(num_trials_same -1) + ((double)1/total_time))/(num_trials_same);
		std::cout << "Mean is " << mean << std::endl;
		same =  std::normal_distribution<double>(mean, (double)1/num_trials_same); 
	} else {
		// Update second.
		num_trials_diff++;
		double mean = different.mean();
		
		mean = (double)(mean*(num_trials_diff -1) + ((double)1/total_time))/(num_trials_diff);
		different = std::normal_distribution<double>(mean, (double)1/num_trials_diff);	
	}

	// Update the curr mode and cpus.
	curr_cpu = 0;
	double num1 = same(generator);
	double num2 = different(generator);
	std::cout << num1 << " " << num2 << std::endl;
	if(num1 > num2) {
		curr_mode = 0;
	} else {
		curr_mode = 1; 
	}
	threads.clear();
	cpus.clear();	 
	std::cout <<  "Current mode: " << curr_mode <<std::endl;
}*/
void remove_distributions()
{
	for (auto it = posteriors.begin(); it != posteriors.end();)
	{

		double mean = it->second.mean();
		int trials = num_trials.at(it->first);

		if (((double)100 / mean > (running_best + 20)) && trials >= 1)
		{
			std::cout << "Deleting";
			num_trials.erase(it->first);
			posteriors.erase(it++);
		}
		else
		{
			++it;
		}
	}

	std::cout << "Current best running mean is: " << running_best << " Size of the action space is: " << posteriors.size() << std::endl;
}

void update_distributions(int total_time)
{

	if (running_best > total_time)
		running_best = total_time;

	std::normal_distribution<double> same = posteriors.at(the_mode);
	int num_trials_same = num_trials.at(the_mode) + 1;
	double mean = same.mean();
	mean = (double)(mean * (num_trials_same - 1) + ((double)100 / total_time)) / (num_trials_same);
	std::cout << "Mean is " << mean << std::endl;
	same = std::normal_distribution<double>(mean, (double)1 / num_trials_same);
	posteriors[the_mode] = same;
	num_trials[the_mode]++;

	if (sample == 0 && posteriors.size() > 10)
	{
		remove_distributions();
		sample = d(generator);
	}
	else
	{
		--sample;
	}
	double max = 0;
	std::cout << "Printing posteriors" << std::endl;
	// Update the mode from the new sample now.
	for (auto m : posteriors)
	{

		// std::cout << m.first << std::endl;
		std::normal_distribution<double> curr_distr = m.second;
		double val = curr_distr(generator);
		if (val > max)
		{
			max = val;
			curr_mode = m.first;
			the_mode = m.first;
		}
	}

	std::cout << "Current mode: " << curr_mode << std::endl;
	/*for(auto m : num_trials) {
		std::cout << "Mode: " << m.first << " " << m.second << std::endl;
	}*/
}

void print_top_1()
{
	double max_mean = 0.0;
	int mode = 0;
	for (auto m : posteriors)
	{
		// std::cout << "Mode: " << m.first << " " << m.second << std::endl;
		double mean = m.second.mean();
		int trials = num_trials.at(m.first);
		if (mean > max_mean)
		{
			max_mean = mean;
			mode = m.first;
		}
	}

	std::cout << "Max mean is: " << max_mean << "Mode is: " << mode << std::endl;
}
