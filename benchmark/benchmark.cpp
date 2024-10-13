#include <benchmark/benchmark.h>
#include "safe_queue.h"
#include "timer.h"
// template <typename T>
// void create(int num,T& q){
//     for(int i =0;i<num;i++){
//         q.push(i);
//     }
// }
// template <typename T>
// void del(int num,T& q){
//     for(int i =0;i<num;i++){
//         q.pop();
//     }
// }
// static void goodversion(benchmark::State& state) {
//   std::vector<std::thread>pool;
//   threadsafe_queue<int>q;
//   int comparison_result = 0;
//   for (auto _ : state) {
//     auto now = syc::Timer();
//     pool.emplace_back(create<threadsafe_queue<int>>,1000,std::ref(q));
//     pool.emplace_back(del<threadsafe_queue<int>>,1000,std::ref(q));
//     for(auto& t:pool){
//         t.join();
//     }
//     std::cout << now.stop() << "ms" << std::endl;
//   }
// }
// BENCHMARK(goodversion);
// static void badversion(benchmark::State& state) {
//   std::vector<std::thread>pool;
//   normal_queue<int>q;
//   int comparison_result = 0;
//   for (auto _ : state) {
//     auto now = syc::Timer();
//     pool.emplace_back(create<normal_queue<int>>,1000,std::ref(q));
//     pool.emplace_back(del<normal_queue<int>>,1000,std::ref(q));
//     for(auto& t:pool){
//         t.join();
//     }
//   }
// }
// BENCHMARK(badversion);
BENCHMARK_MAIN();