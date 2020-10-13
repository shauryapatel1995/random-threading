#include <iostream>
#include <fstream>
#include <list>
#include <chrono>
#include <unordered_map>
#include "threads.h"
#include <pthread.h>
using namespace std;

typedef std::chrono::high_resolution_clock Clock;

extern void create_workload(list<int>& workload);
extern void gups();
extern unsigned int ackermann(unsigned int m, unsigned int n);
extern int foo();
extern int getCurrentID();

class workloadUnit {
    int func_name; 
    double time_taken;

    public :
    workloadUnit(int func, double time) {
        this->func_name = func;
        this->time_taken = time;
    }

    int getFunc() {
        return this->func_name;
    }

    double getTime() {
        return this->time_taken;
    }
};

unordered_map<int, workloadUnit *> runtimes;


void run_benchmark(int func_num) {
    
    auto t1 = Clock::now();

    if(func_num == 1) {
        foo();
    } else if(func_num == 2) {
        gups();
    } else {
        ackermann(rand() % 4, rand() % 10);
    }
    auto t2 = Clock::now();
    workloadUnit *work = new workloadUnit(func_num, chrono::duration_cast<chrono::milliseconds>(t2 - t1).count());
    runtimes.insert(pair<int, workloadUnit *>(pthread_self(), work));
    
}


void read_workload(list<int>& workload, char* file) {
    cout << "File is" << file << std::endl;
    ifstream in(file);
    std::string str;
    while(std::getline(in, str)) {
        workload.push_back(stoi(str));
    }
}

int main(int argc, char** argv)
{
    // Load workload file
    
    auto t1 = Clock::now();
    list<int> workload;
    // create_workload(workload);
    char* file = argv[1];
    //create_workload(workload);
    read_workload(workload, file);

    cout << "Random workload of size " << workload.size() << " created\n";
    
    // Run this workload
    cout << "Running workload" << std::endl;
    list<int> threads;
    for (int i = 0; i < 100; i++)
    {
        int work = workload.front();
        int thread_id = thread_create(run_benchmark, work);
        threads.push_back(thread_id);
        workload.pop_front();
    }

    //joinAllThreads();
    for(auto t : threads) {
        join_thread(t);
    }
        
    // std::this_thread::sleep_for(10s);
    
    cout << "\nPrinting time taken : \n";
    for (auto i : runtimes)
    {
        cout << i.second->getFunc() << " " << i.second->getTime() << "\n";
    }

    auto t2 = Clock::now();
    cout << "Total time taken is: " << chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
    
}
