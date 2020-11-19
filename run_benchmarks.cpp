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
int size = 4096000;
void run_benchmark(int * args) {
    int func_num = args[0];
    int size = args[1];
    auto t1 = Clock::now();

    if(func_num == 1) {
        foo();
    } else if(func_num == 2) {
        gups(size);
    } else if(func_num == 3) {
	gups_pareto(size);
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
    int size_mem = atoi(argv[2]);
    //create_workload(workload)
    // Run this workload
    // cout << "Running workload " << std::endl;
    double average[4] = {0};
    double running_time = 0.0;
for(int i = 0; i < 100; i++) {
 read_workload(workload, file);
    cout << "Random workload of size " << workload.size() << " created\n";
    auto t1 = Clock::now();

    list<pthread_t> threads;
    int size = workload.size();
    for (int i = 0; i < size; i++)
    {
        int work = workload.front();
	// threads.push_back(std::thread(run_benchmark, work));
	int * args = (int *)malloc(sizeof(int)*2);
	args[0] = work; 
	args[1] = size_mem;
	pthread_t thread;
	pthread_create(&thread, NULL, run_benchmark, args);
	threads.push_back(thread);
        workload.pop_front();
    }

    for(auto& t : threads)
       	pthread_join(t, nullptr);

    
    auto t2 = Clock::now();
    int total_time = chrono::duration_cast<chrono::milliseconds>(t2 - t1).count();
    cout << "Printing time taken : \n";
    int j = 0;
    for (auto k : runtimes)
    {
        cout << k.second->getFunc() << " " << k.second->getTime() << "\n";
	average[j] = (double)(average[j] + k.second->getTime());
        j++;

    }
    runtimes.clear();
    threads.clear();
    running_time += total_time;
    cout << "Total time taken: " << total_time << "\n";
    }
	cout << "Running time average: " << (double) running_time / 100 << std::endl;
	for(int j = 0; j < 4; j++) {
                cout << "Average runtime of thread j is: " << (double) average[j] / 100 << endl;
        }

}
