#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <stdlib.h>
#include <list>
#include <time.h>

using namespace std;

std::unordered_map<std::thread::id, double> thread_times;

void foo() {
    
    int i;
    unsigned int out = 0;
    unsigned int a = static_cast<unsigned int>(time(NULL));
    
    for (i = 0; i < 1000; i++)
    {
        // something
        out += rand_r(&a);
    }
    printf("%d\n", out);
    
}