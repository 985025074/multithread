#pragma once
#include "std.h"
#include "helper.h"
//报的这个illegalinstruction 可能是返回值没有
template <typename IterBegin,typename IterEnd,typename TargetType>
void find_single(IterBegin begin,IterEnd end,TargetType target,std::atomic<bool>& done,std::promise<IterBegin>&result){
    try{
        for(auto iter=begin;(!done.load())&&iter!=end;iter++){
            if(*iter==target){
                done = true;
                result.set_value(iter);

            }
        }
    
    } 
    catch(...){
        done =true;
        result.set_exception(std::current_exception());
        
    }
}
template <typename IterBegin,typename IterEnd,typename TargetType>
IterBegin parallel_find(IterBegin begin,IterEnd end,TargetType target){
    std::atomic<bool> done(false);
    unsigned int num_threads = std::thread::hardware_concurrency();
    auto tasks_number = std::distance(begin,end);
    const int task_per_thread = 25;
    auto thread_need_hope = (tasks_number +task_per_thread-1)/task_per_thread;
    //the exact number of threads used finally
    auto thread_final = std::min(num_threads,static_cast<unsigned int>(thread_need_hope));
    //finally,the number of each thread's task
    auto task_per_thread_final = tasks_number/thread_final;
    std::promise<IterBegin> result;
    {
        std::vector<join_thread> threads(thread_final);
        
        for(unsigned int i=0;i<thread_final;i++){
            IterBegin task_begin = begin;
            std::advance(begin,task_per_thread_final);
            IterEnd task_end =begin;
            threads[i] = join_thread(find_single<IterBegin,IterEnd,TargetType>,task_begin,task_end,target,std::ref(done),std::ref(result));
        }
        find_single<IterBegin,IterEnd,TargetType>(begin,end,target,done,result);
    }
    auto ret= result.get_future();
    if(!done.load()){
        return end;
    }
    return ret.get();
}