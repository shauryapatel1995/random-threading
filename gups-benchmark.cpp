#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <ctime>
typedef std::chrono::high_resolution_clock Clock;

using namespace std; 

int get_next_index(int i, int size) {
    unsigned int time = (unsigned int)std::time(nullptr);
    return rand_r(&time) % size;
}

void gups() {

    int size = 409600;
    int *field = (int *) malloc(size);
    
    
    
    int data; 
    unsigned long i; 
    unsigned long long index; 
    unsigned long iters = 1000000;
    size = 8;
    for(int i = 0; i < iters; i++) {
        
        index = get_next_index(i, size);
        data = field[index];
        data += iters;
        field[index] = data;
    }
    
    
}

// int main() {
 
    
//     auto t1 = Clock::now();
//     gups();
//     auto t2 = Clock::now();
//     cout << chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
//     std::cout << "Finished!"
//               << "\n";
// }