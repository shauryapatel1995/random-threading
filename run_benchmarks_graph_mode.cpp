#include <iostream>
#include <fstream>
#include <list>
#include <chrono>
#include <unordered_map>
#include "threads.h"
#include "graph-threads.h"
#include <pthread.h>

using namespace std;

typedef std::chrono::steady_clock Clock;

extern void create_workload(list<int> &workload);
extern int gups(int size);
extern unsigned int ackermann(unsigned int m, unsigned int n);
extern int foo();
extern int getCurrentID();
extern int gups_pareto(int size);

class workloadUnit
{
    int func_name;
    double time_taken;
    int cpu_num;

public:
    workloadUnit(int func, double time, int cpu)
    {
        this->func_name = func;
        this->time_taken = time;
        this->cpu_num = cpu;
    }

    int getFunc()
    {
        return this->func_name;
    }

    double getTime()
    {
        return this->time_taken;
    }

    int getCpu()
    {
        return this->cpu_num;
    }
};

unordered_map<int, workloadUnit *> runtimes;

void run_benchmark(int *args)
{
    int func_num = args[0];
    int size = args[1];
    int ans = 0;
    auto t1 = Clock::now();
    // cout << "t1 " << std::chrono::system_clock::to_time_t(t1) << endl;
    // cout << "Function is " << func_num << endl;
    // cout << "Size of memory is " << size << endl;
    if (func_num == 1)
    {
        foo();
    }
    else if (func_num == 2)
    {
        // cout << "gups" << endl;
        ans = gups(size);
    }
    else if (func_num == 3)
    {
        // cout << "pareto" << endl;
        ans = gups_pareto(size);
    }
    else
    {
        ackermann(rand() % 4, rand() % 10);
    }
    auto t2 = Clock::now();
    // cout << "t2 " << std::chrono::system_clock::to_time_t(t2) << endl;

    // workloadUnit *work = new workloadUnit(func_num, chrono::duration_cast<chrono::milliseconds>(t2 - t1).count(), get_cpu(pthread_self()));
    // runtimes.insert(pair<int, workloadUnit *>(pthread_self(), work));
}

void read_workload(list<int> &workload, char *file)
{
    cout << "File is" << file << std::endl;
    ifstream in(file);
    std::string str;
    while (std::getline(in, str))
    {
        workload.push_back(stoi(str));
    }
}

int main(int argc, char **argv)
{
    // Load workload file

    list<int> workload;
    // create_workload(workload);
    char *file = argv[1];
    int size_mem = atoi(argv[2]);
    int cpus = 0;
    bool train = true;

    cout << "Size is: " << size_mem << endl;
    int runtime = 0;
    // generate_posteriors(4);
    double average[4] = {0.0};
    //create_workload(workload)

    read_workload(workload, file);

    cout << "Train Random workload of size " << workload.size() << " created\n";

    // Run this workload
    cout << "Running workload" << std::endl;
    list<int> threads;

    auto time1 = Clock::now();
    int size = workload.size();
    for (int i = 0; i < size; i++)
    {
        int work = workload.front();
        int *args = (int *)malloc(sizeof(int) * 2);
        args[0] = work;
        args[1] = size_mem;
        int thread_id = thread_create(run_benchmark, (void *)args, work);
        threads.push_back(thread_id);
        workload.pop_front();
    }

    run_experiments();
    // for (auto t : threads)
    // {
    //     join_thread(t);
    // }

    // std::this_thread::sleep_for(10s);

    auto time2 = Clock::now();
    // cout << "\nPrinting time taken : \n";
    /*int j = 0;
    for (auto k : runtimes)
    {
        cout << k.first << " " << k.second->getFunc() << " " << k.second->getTime()  << " " << k.second->getCpu() << "\n";
	if(i >= 500) {
		average[j] = (double)(average[j] + k.second->getTime());
		j++;
	}
    } */
    // delete(&runtimes);
    // runtimes = unordered_map<int, workloadUnit *>();
    runtimes.clear();
    threads.clear();
    int total_time = chrono::duration_cast<chrono::milliseconds>(time2 - time1).count();
    cout << "Total time taken is: " << total_time << endl;
    
    
}


