#pragma once
#include "std.h"
#include "helper.h"

template <typename IterBegin,typename IterEnd,typename Func>
void parallel_for_each(IterBegin begin,IterEnd end,Func func){
    unsigned int num_threads = std::thread::hardware_concurrency();
    auto tasks_number = std::distance(begin,end);
    const int task_per_thread = 25;
    auto thread_need_hope = (tasks_number +task_per_thread-1)/task_per_thread;
    //the exact number of threads used finally
    auto thread_final = std::min(num_threads,static_cast<unsigned int>(thread_need_hope));
    //finally,the number of each thread's task
    auto task_per_thread_final = tasks_number/thread_final;
    //first_way use package_task + future combination
    std::vector<std::future<void>> futures(thread_final);
    std::vector<join_thread> threads(thread_final);
    {
        for(int i=0;i<thread_final;i++){
            IterBegin task_begin = begin;
            std::advance(begin,task_per_thread_final);
            IterEnd task_end =begin;
            std::packaged_task<void()>temp([task_begin,task_end,func](){
                std::for_each(task_begin,task_end,func);
            });
            futures[i] = temp.get_future();
            threads[i] = join_thread(std::move(temp));
            
        }
    }
    std::for_each(begin,end,func);
    for(auto &f:futures){
        f.get();
    }
    return;
}

//test->
    // std::vector<int> a(1000000);
    // syc::Timer t;
    // parallel_for_each(a.begin(),a.end(),[](int& x){x++;});
    // std::cout << "parallel_for_each time:" << t.stop() << std::endl;
    // std::for_each(a.begin(),a.end(),[](int& x){x++;});
    // std::cout << "std_for_each time:" << t.stop() << std::endl;
template <typename IterBegin,typename IterEnd,typename Func>
void parallel_for_each_recursive(IterBegin begin,IterEnd end,Func func){  
    auto tasks_number = std::distance(begin,end);
    const int task_per_thread = 25;
    if(tasks_number == 0){
        return;
    }
    if(tasks_number<=task_per_thread){
        std::for_each(begin,end,func);
        return;
    }
    else{
        IterEnd task_end = begin + task_per_thread;
        std::future<void>f = std::async([begin,task_end,func](){
            std::for_each(begin,task_end,func);
        }
        );
        begin = task_end;
        parallel_for_each_recursive(begin,end,func);
        f.get();
    }
}  