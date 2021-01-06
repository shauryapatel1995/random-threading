# random-threading
g++ -I. run_benchmarks_user_mode.cpp random-threads.cpp gups-benchmar
k.cpp normal_benckmark.cpp ackerman.cpp -lpthread -fpermissive

g++ -I. -std=c++11 -o rand run_benchmarks_user_mode.cpp random-threads.cpp gups-benchmark.cpp normal_benckmark.cpp ackerman.cpp gups-pareto.cpp -lpthread -fpermissive
