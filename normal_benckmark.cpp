#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <stdlib.h>
#include <list>
#include <time.h>

using namespace std;

int n = 500000;
std::unordered_map<std::thread::id, double> thread_times;

int foo() {
    
    int arr[n];

    unsigned int a = static_cast<unsigned int>(time(NULL));
    unsigned long out = 0;

    for(int i = 0; i < n; i++) 
        arr[i] = rand_r(&a);

    for (int i = 0; i < n; i++)
    {
        // something
        out += arr[i];
    }
   	
    return out;
    
}
