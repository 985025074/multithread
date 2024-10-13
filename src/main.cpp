// #include "myPool.h"
// struct test :public test_base{
//     std::atomic<int> x = 0;
//     char space[64];
//     std::atomic<int> y = 0;
//     void entry(tag<0>){
//         x.store(1,std::memory_order_relaxed);
//         y.store(1,std::memory_order_relaxed);
//     }
//     void entry(tag<1>){
//        while(!y.load(std::memory_order_relaxed));
//        _global = x;
//     }
//     //结果统计者

// };
// int main() {
//     #ifdef __aarch64__
//         std::cout << "aarch64" << std::endl;
//     #endif
//     runtest<test,10000>();

//     return 0;
// }
