// #include "std.h"
// #include "helper.h"
// #include "my_parallel_for_each.h"
// #include "my_parallel_find.h"
// #include "my_parallel_partial_sum.h"
// #include "threadpool.h"
// using namespace std::literals;
// template<typename T>
// std::list<T> quicksort_traditional(std::list<T>&&lis) {
//     if (lis.size() <= 1) {
//         return lis;
//     }
//     std::list<T> left_part;
//     T mid_val = lis.front();
//     lis.pop_front();
//     auto midright = std::partition(lis.begin(), lis.end(), [mid_val](const T& x) { return x < mid_val; });
//     left_part.splice(left_part.end(), lis, lis.begin(), midright);
//     std::future<std::list<T>> f = std::async(std::launch::async, quicksort_traditional<T>, std::move(left_part));

//     auto right_part = quicksort_traditional(std::move(lis));
//     left_part = f.get();
//     left_part.insert(left_part.end(), mid_val);
//     left_part.insert(left_part.end(), right_part.begin(), right_part.end());

//     return left_part;
// }
// //use our v2 version to make it cooler
// thread_pool_v2 t1(10);
// template<typename T>
// std::list<T> quicksort_threadpool(std::list<T>&&lis) {
//     if (lis.size() <= 1) {
//         return lis;
//     }
//     std::list<T> left_part;
//     T mid_val = lis.front();
//     lis.pop_front();
//     auto midright = std::partition(lis.begin(), lis.end(), [mid_val](const T& x) { return x < mid_val; });
//     left_part.splice(left_part.end(), lis, lis.begin(), midright);
//     auto left_future = t1.add_task([left_part=std::move(left_part)]()mutable{return quicksort_threadpool<T>(std::move(left_part));});
//     auto right_future= t1.add_task([lis=std::move(lis)]()mutable{return quicksort_threadpool<T>(std::move(lis));});
//     while(left_future.wait_for(0ms) == std::future_status::timeout || right_future.wait_for(0ms) == std::future_status::timeout){
//         t1.run_task();
//     }
//     left_part = left_future.get();
//     auto right_part = right_future.get();   
//     left_part.insert(left_part.end(), mid_val);
//     left_part.insert(left_part.end(), right_part.begin(), right_part.end());

//     return left_part;
// }
// int main(){
//     std::list<int> lis(1000,1);
//     syc::Timer t;
//     auto temp1 =quicksort_traditional(std::list<int>(lis));
//     std::cout << "traditional time:" << t.stop() << std::endl;
//     auto temp2 =quicksort_threadpool(std::list<int>(lis));
//     std::cout << "threadpool time:" << t.stop() << std::endl;
   
//     return 0;
// }
