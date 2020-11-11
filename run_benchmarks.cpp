#include <iostream>
#include <fstream>
#include <list>
#include <chrono>
#include <unordered_map>
#include <thread>
using namespace std;

typedef std::chrono::high_resolution_clock Clock;

// extern void create_workload(list<int>& workload);
extern int gups(int);
extern int gups_pareto(int);
extern unsigned int ackermann(unsigned int m, unsigned int n);
extern void foo();

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

unordered_map<std::thread::id, workloadUnit *> runtimes;

void run_benchmark(int func_num) {
    
    auto t1 = Clock::now();

    if(func_num == 1) {
        foo();
    } else if(func_num == 2) {
        gups(4096000);
    } else if(func_num == 3) {
	gups_pareto(4096000);
    } else {
        ackermann(rand() % 4, rand() % 10);
    }
    auto t2 = Clock::now();
    workloadUnit *work = new workloadUnit(func_num, chrono::duration_cast<chrono::milliseconds>(t2 - t1).count());
    runtimes.insert(pair<thread::id, workloadUnit *>(this_thread::get_id(), work));
    return; 
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
    
    list<int> workload;
    char* file = argv[1];
    //create_workload(workload);
    read_workload(workload, file);
    cout << "Random workload of size " << workload.size() << " created\n";
    int running_time = 0;
    auto t1 = Clock::now();    
    // Run this workload
    // cout << "Running workload " << std::endl;
for(int i = 0; i < 100; i++) {
    list<thread> threads;
    int size = workload.size();
    for (int i = 0; i < size; i++)
    {
        int work = workload.front();
	threads.push_back(std::thread(run_benchmark, work));
        workload.pop_front();
    }

    for(auto& t : threads)
        t.join();

    
    auto t2 = Clock::now();
    int total_time = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
    cout << "Printing time taken : \n";
    for (auto i : runtimes)
    {
        cout << i.second->getFunc() << " " << i.second->getTime() << "\n";
    }
	running_time += total_time;
    cout << "Total time taken: " << total_time << "\n";
    }
	cout << "Running time average: " << (double) running_time / 100 << std::endl;
}
