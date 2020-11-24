#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <stdlib.h>
#include <list>
#include <time.h>
#include <mutex>

using namespace std;

int n = 500000;
// std::unordered_map<std::thread::id, double> thread_times;
// unsigned long out = 0;
std::mutex mtx;

int foo() {
    
    int arr[n];
    unsigned long out = 0; 
    unsigned int a = static_cast<unsigned int>(time(NULL));

    for(int i = 0; i < n; i++) 
        arr[i] = 0;

    for (int i = 0; i < n; i++)
    {
        // something
	 //mtx.lock();
        out += arr[i];
	// mtx.unlock();
    }
   	
    return out;
    
}
