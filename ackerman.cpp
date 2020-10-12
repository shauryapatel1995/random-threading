// Highly compute intensive ackerman function
#include <iostream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;

using namespace std; 

unsigned int ackermann(unsigned int m, unsigned int n)
{
	
	if (m == 0)
    {
        return n + 1;
    }
    if (n == 0)
    {
        return ackermann(m - 1, 1);
    }
	
	return ackermann(m - 1, ackermann(m, n - 1));
}

// int main()
// {
//     for (unsigned int m = 0; m < 4; ++m)
//     {
//         for (unsigned int n = 0; n < 10; ++n)
//         {
// 			auto t1 = Clock::now();
			
// 			std::cout << "A(" << m << ", " << n << ") = " << ackermann(m, n) << "\n";
// 			auto t2 = Clock::now();
// 			cout << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count() << "\n";
			
// 		}
//     }
// }