#pragma once
// #include "timer.h"
#include <thread>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <iostream>
#include <future>
namespace details{
    std::vector<std::thread> threads;
}
using result_map = std::map<std::string,unsigned>;
template <unsigned T>
struct tag{ 
    static unsigned constexpr num = T;    
};

template <typename T,unsigned U,typename = void>
struct entry_value{
  static unsigned constexpr value = entry_value<T,U+1>::value;
};
template <typename T,unsigned U>
requires (!requires (T test){
    {test.entry(tag<U>{})};
})
struct entry_value<T,U,void>{
    static unsigned constexpr value = U;
};
//静态循环代码，beginend 分别代码起始终止值，functemp 循环体，func_args 循环参数
template <unsigned Begin,unsigned End,template <unsigned ,typename...>class functemp,typename...func_args>
void static_for(func_args...args){
    if constexpr(Begin == End){
        return;
    }
    else{
        functemp<Begin,func_args...>()(args...);
        static_for<Begin+1,End,functemp,func_args...>(args...);
    }
}
void set_core(std::thread& thread,int i){
#ifdef __x86_64__
     cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset);
    int rc = pthread_setaffinity_np(thread.native_handle(),
                                    sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
      std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
#endif
#ifdef __aarch64__
 cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(i, &cpuset);
    
    // 获取线程的 TID，使用 Linux syscall 来获取
    pid_t tid = gettid(); // 对于某些系统，你可能需要使用 syscall(SYS_gettid)

    // 设置线程的 CPU 亲和性
    int rc = sched_setaffinity(tid, sizeof(cpu_set_t), &cpuset);
    if (rc != 0) {
        std::cerr << "Error calling sched_setaffinity: " << rc << "\n";
    }
#endif
}

template <unsigned i,typename Pointer,typename ShareFuture>
struct push_thread{

    void operator()(Pointer pointer,ShareFuture start_message){//等待start message
    auto lambda = [start_message =std::move(start_message)](Pointer pointer,auto arg){start_message.wait();pointer->entry(arg);};
    details::threads.push_back(std::thread(lambda,pointer,tag<i>{}));
    set_core(details::threads.back(),i);

    }
};
template <typename T>
auto runtest_per(T&&test){
    std::promise<void>p;
    auto wait_this = p.get_future().share();
    constexpr unsigned num_threads =entry_value<T,0>::value;
    details::threads.clear();
    static_for<0,num_threads,push_thread>(&test,wait_this);
    //通知开工
    p.set_value();
    for(auto& t:details::threads){
        t.join();   
    }
    return test.result();//result 应该返回一个map 记录结果对应值次数
}
template <typename T,unsigned times>
void runtest(){
    constexpr unsigned num_threads =entry_value<T,0>::value;
    std::cout <<"num_threads:"<<num_threads <<"\n";
    std::map<std::string,unsigned>count_result;
    for(int i=0;i<times;i++){
        auto result =runtest_per<T>(T());
        for(auto& item:result){
            count_result[item.first]+=item.second;
        }
    }
    for(auto& item:count_result){
      std::cout << item.first << ":" << item.second << "\n";
    }
}
struct test_base{
    int _global = 0;
    result_map result(){
        result_map _;
        _[std::to_string(_global)] = 1;
        return _;
    }
};
