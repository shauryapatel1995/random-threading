#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <unistd.h>
typedef std::chrono::high_resolution_clock Clock;

using namespace std; 

unsigned long long get_next_index(int i, int size) {
    unsigned int a = static_cast<unsigned int>(time(NULL) + pthread_self() + i);
    return rand_r(&a) % size;
}

void gups() {

    int size = 46080000;
    int * field = (int *) malloc(sizeof(int)*size);
    
    
    
    int data; 
    unsigned long i; 
    unsigned long long index; 
    unsigned long iters = 10000000;
	
    
    for(int i = 0; i < iters; i++) {
    	    
        index = get_next_index(i, size);

	if(index > size) {
		cout << index << " " << size << endl;
	}
        data = field[index];
        data += iters;
        field[index] = data;
    }
  
    free(field); 
    
}

// int main() {
 
    
//     auto t1 = Clock::now();
//     gups();
//     auto t2 = Clock::now();
//     cout << chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
//     std::cout << "Finished!"
//               << "\n";
// }
