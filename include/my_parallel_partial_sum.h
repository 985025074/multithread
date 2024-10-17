//性能一般，不如标准库
#pragma once
#include "std.h"
#include "helper.h"
template <typename BeginIter,typename EndIter,typename ElementType =  std::decay_t<decltype(*std::declval<BeginIter>())>>
void parallel_partial_sum_impl(BeginIter begin, EndIter end,std::future<ElementType>*front_value,std::promise<ElementType>*back_value) {
    ElementType init = 0;
    if(front_value != nullptr)[[likely]]{
        init = front_value->get();
    }
    std::partial_sum(begin,end,begin);
    auto &final_val = *(end-1);
    final_val += init;
    back_value->set_value(final_val);
    std::for_each(begin,end-1,[init](ElementType&x){x+=init;});
}
template <typename BeginIter,typename EndIter,typename ElementType = std::decay_t<decltype(*std::declval<BeginIter>())>>   
std::vector<ElementType> parallel_partial_sum(BeginIter begin, EndIter end) {
    //create a copy
    std::vector<ElementType> copy_(begin,end);
    begin = copy_.begin();
    end = copy_.end();
    
    unsigned int num_threads = std::thread::hardware_concurrency();
    auto tasks_number = std::distance(begin,end);
    const int task_per_thread = 50000;
    auto thread_need_hope = (tasks_number +task_per_thread-1)/task_per_thread;
    //the exact number of threads used finally
    auto thread_final = std::min(num_threads,static_cast<unsigned int>(thread_need_hope));
    //finally,the number of each thread's task
    auto task_per_thread_final = tasks_number/thread_final;
    std::vector<std::promise<ElementType>>promises(thread_final+1);//see below
    std::vector<std::future<ElementType>>futures(thread_final+1);//define the last future for the final result.
    std::vector<std::future<void>>threads(num_threads);

    for(int i = 0; i < thread_final; i++){
        BeginIter task_begin = begin;
        std::advance(begin,task_per_thread_final);
        EndIter task_end =begin;
        threads[i] = std::async(parallel_partial_sum_impl<BeginIter,EndIter>,task_begin,task_end,i==0?nullptr:&futures[i-1],&promises[i]);
        
        futures[i] = promises[i].get_future();
    }
    if(begin != end)[[likely]]{
        parallel_partial_sum_impl(begin,end,&futures[thread_final-1],&promises[thread_final]);
        promises[thread_final].get_future().wait();
    }
    else [[unlikely]]{
        futures[thread_final-1].wait();
    } 
    return copy_;
}