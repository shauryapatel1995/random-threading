#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <random>

typedef std::chrono::high_resolution_clock Clock;

using namespace std;

/*unsigned long long get_next_index(int i, int size, std::uniform_int_distribution<int> distr, std::mt19937 generator) {
    return distr(generator);
}*/

int gups(int size)
{

    int *field = (int *)malloc(sizeof(int) * size);
    std::random_device rand_dev;
    std::mt19937 generator(rand_dev());
    std::uniform_int_distribution<> distr(1, size);
    // boost::math::pareto_distribution<> dist(1, 1);
    // boost::variate_generator<boost::mt19937&,boost::random::uniform_int_distribution<> > generator(randGen, dist);
    //    cout << "Created distribution of size " << size << endl;

    int data;
    unsigned long i;
    unsigned long long index;
    unsigned long iters = 1000000;

    for (int i = 0; i < iters; i++)
    {

        index = distr(generator);
        // cout << index << endl;
        //double val = boost::math::cdf(complement(dist,distr(generator)));
        //index = (int)(val * size);
        // cout << index << endl;

        if (index > size)
        {
            cout << index << " " << size << endl;
        }
        //	cout << index << endl;
        data = field[index];
        data += iters;
        field[index] = data;
    }

    free(field);
    return data;
}

// int main() {

//     auto t1 = Clock::now();
//     gups();
//     auto t2 = Clock::now();
//     cout << chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
//     std::cout << "Finished!"
//               << "\n";
// }
