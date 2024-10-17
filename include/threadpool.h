//my thread pool
#pragma once
#include "std.h"
#include "helper.h"
#include "safe_queue.h"
class thread_pool_v1{
    private:
    //mention the order! threads need to be the last.
    std::atomic<bool> done;
    normal_queue<std::function<void()>> tasks;
    std::vector<join_thread> threads;
    
    void work_thread(){
        std::shared_ptr<std::function<void()>> task_ptr;
        while(!done){
            if(task_ptr = std::move(tasks.try_pop())){
                (*task_ptr)();
            }
            else{
                std::this_thread::yield();
            }
        }
    }
    public:
    thread_pool_v1(int num_threads):done(false),threads(num_threads){
        //we use join thread. so we dont need to do like in the book
        for(int i=0;i<num_threads;i++){
            threads[i] = join_thread([this](){work_thread();});
        }
    }
    thread_pool_v1(const thread_pool_v1&) = delete;
    thread_pool_v1& operator=(const thread_pool_v1&) = delete;
    thread_pool_v1(thread_pool_v1&&) = default;
    thread_pool_v1& operator=(thread_pool_v1&&) = default;
    //default move constructor and move assignment are fine
    
    void add_task(std::function<void()> task){
        tasks.push(std::move(task));
    }
    ~thread_pool_v1(){
        done = true;
    }
};

//version 2
//make wait for particular task possoble
namespace details{
    class func_base{
        public:
        func_base(){
            
        }
        virtual void call() =0; 
        //move constructor and move assignment are fine default
        func_base(func_base&&) = default;
        func_base& operator=(func_base&&) = default;
        virtual ~func_base(){

        }
    };
    template <typename F>
    class concrete_func_base: public func_base
    {
        private:
        F f;
        public:
        concrete_func_base(F&&f):f(std::move(f)){

        }
        //default move constructor and move assignment are fine 
        concrete_func_base(concrete_func_base&&) = default;
        concrete_func_base& operator=(concrete_func_base&&) = default;
        void call() override{
            f();
        }
        ~concrete_func_base() override{

        }

    };
    class func_wrapper{
        private:
        std::unique_ptr<func_base> f;
        public:
        template <typename F>
        func_wrapper(F&&f):f(std::make_unique<concrete_func_base<F>>(std::forward<F>(f))){
        }
        void operator()(){
            f->call();
        }
        //default move constructor and move assignment are fine 
        func_wrapper(func_wrapper&&) = default;
        func_wrapper& operator=(func_wrapper&&) = default;
        //auto delete copy constructor and copy assignment
    };

}

class thread_pool_v2{
    private:

    //mention the order! threads need to be the last.
    using FuncContainer = details::func_wrapper;
    std::atomic<bool> done;
    normal_queue<FuncContainer> tasks;
    std::vector<join_thread> threads;
    
    void work_thread(){
        std::shared_ptr<FuncContainer> task_ptr;
        while(!done){
            if(task_ptr = std::move(tasks.try_pop())){
                (*task_ptr)();
            }
            else{
                std::this_thread::yield();
            }
        }
    }
    public:
    thread_pool_v2(int num_threads):done(false),threads(num_threads){
        //we use join thread. so we dont need to do like in the book
        for(int i=0;i<num_threads;i++){
            threads[i] = join_thread([this](){work_thread();});
        }
    }
    thread_pool_v2(const thread_pool_v2&) = delete;
    thread_pool_v2& operator=(const thread_pool_v2&) = delete;
    thread_pool_v2(thread_pool_v2&&) = default;
    thread_pool_v2& operator=(thread_pool_v2&&) = default;
    //default move constructor and move assignment are fine
    
    template <typename F,typename Ret = decltype(std::declval<F>()())>
    std::future<Ret> add_task(F f){
        std::packaged_task<Ret()> task(std::move(f));
        std::future<Ret> ret = task.get_future();
        tasks.push(std::move(FuncContainer(std::move(task))));
        return ret;
    }
    ~thread_pool_v2(){
        done = true;
    }
};