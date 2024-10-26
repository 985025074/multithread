// my thread pool
#pragma once
#include "std.h"
#include "helper.h"
#include "safe_queue.h"
class thread_pool_v1
{
private:
    // mention the order! threads need to be the last.
    std::atomic<bool> done;
    normal_queue<std::function<void()>> tasks;
    std::vector<join_thread> threads;

    void work_thread()
    {
        std::shared_ptr<std::function<void()>> task_ptr;
        while (!done)
        {
            if (task_ptr = std::move(tasks.try_pop()))
            {
                (*task_ptr)();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool_v1(int num_threads) : done(false), threads(num_threads)
    {
        // we use join thread. so we dont need to do like in the book
        for (int i = 0; i < num_threads; i++)
        {
            threads[i] = join_thread([this]()
                                     { work_thread(); });
        }
    }
    thread_pool_v1(const thread_pool_v1 &) = delete;
    thread_pool_v1 &operator=(const thread_pool_v1 &) = delete;
    thread_pool_v1(thread_pool_v1 &&) = default;
    thread_pool_v1 &operator=(thread_pool_v1 &&) = default;
    // default move constructor and move assignment are fine

    void add_task(std::function<void()> task)
    {
        tasks.push(std::move(task));
    }
    ~thread_pool_v1()
    {
        done = true;
    }
};

// version 2
// make wait for particular task possoble
namespace details
{
    class func_base
    {
    public:
        func_base()
        {
        }
        virtual void call() = 0;
        // move constructor and move assignment are fine default
        func_base(func_base &&) = default;
        func_base &operator=(func_base &&) = default;
        virtual ~func_base()
        {
        }
    };
    template <typename F>
    class concrete_func_base : public func_base
    {
    private:
        F f;

    public:
        concrete_func_base(F &&f) : f(std::move(f))
        {
        }
        // default move constructor and move assignment are fine
        concrete_func_base(concrete_func_base &&) = default;
        concrete_func_base &operator=(concrete_func_base &&) = default;
        void call() override
        {
            f();
        }
        ~concrete_func_base() override
        {
        }
    };
    class func_wrapper
    {
    private:
        std::unique_ptr<func_base> f;

    public:
        func_wrapper() = default;
        template <typename F>
        func_wrapper(F &&f) : f(std::make_unique<concrete_func_base<F>>(std::forward<F>(f)))
        {
        }
        void operator()()
        {
            f->call();
        }
        // default move constructor and move assignment are fine
        func_wrapper(func_wrapper &&) = default;
        func_wrapper &operator=(func_wrapper &&) = default;
        // auto delete copy constructor and copy assignment
    };

}

class thread_pool_v2
{
private:
    // mention the order! threads need to be the last.
    using FuncContainer = details::func_wrapper;
    std::atomic<bool> done;
    normal_queue<FuncContainer> tasks;
    std::vector<join_thread> threads;
    void work_thread()
    {
        std::shared_ptr<FuncContainer> task_ptr;
        while (!done)
        {
            if (task_ptr = std::move(tasks.try_pop()))
            {
                (*task_ptr)();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool_v2(int num_threads) : done(false), threads(num_threads)
    {
        // we use join thread. so we dont need to do like in the book
        for (int i = 0; i < num_threads; i++)
        {
            threads[i] = join_thread([this]()
                                     { work_thread(); });
        }
    }
    thread_pool_v2(const thread_pool_v2 &) = delete;
    thread_pool_v2 &operator=(const thread_pool_v2 &) = delete;
    thread_pool_v2(thread_pool_v2 &&) = default;
    thread_pool_v2 &operator=(thread_pool_v2 &&) = default;
    // default move constructor and move assignment are fine

    template <typename F, typename Ret = decltype(std::declval<F>()())>
    std::future<Ret> add_task(F &&f)
    {
        std::packaged_task<Ret()> task(std::forward<F>(f));
        std::future<Ret> ret = task.get_future();
        tasks.push(std::move(FuncContainer(std::move(task))));
        return ret;
    }
    void run_task()
    {   std::shared_ptr<FuncContainer> task_ptr;
        if (task_ptr = std::move(tasks.try_pop()))
        {
            (*task_ptr)();
        }
        else
        {
            std::this_thread::yield();
        }
    }
    ~thread_pool_v2()
    {
        done = true;
    }
};
//add task with local queue to improve pingpong cache //relax the frequent lock
class thread_pool_v3
{
private:
    // mention the order! threads need to be the last.
    using FuncContainer = details::func_wrapper;
    std::atomic<bool> done;
    normal_queue<FuncContainer> tasks;//global
    using local_queue = std::queue<std::shared_ptr<FuncContainer>>;
    inline static thread_local std::unique_ptr<local_queue> local_tasks ;//to decrease the size,we use ptr.
    std::vector<join_thread> threads;
    void work_thread()
    {
        std::shared_ptr<FuncContainer> task_ptr;
        while (!done)
        {
            if (local_tasks && !local_tasks->empty()){
                task_ptr = std::move(local_tasks->front());
                local_tasks->pop();
                (*task_ptr)();
            }
            else if (task_ptr = std::move(tasks.try_pop()))
            {
                (*task_ptr)();
            }
            else
            {
                std::this_thread::yield();
            }
        }
    }

public:
    thread_pool_v3(int num_threads) : done(false), threads(num_threads)
    {
        // we use join thread. so we dont need to do like in the book
        for (int i = 0; i < num_threads; i++)
        {   //add initialization of the localqueue
            threads[i] = join_thread([this]() 
                                     { local_tasks = std::make_unique<local_queue>(); work_thread(); });
        }
    }
    thread_pool_v3(const thread_pool_v3 &) = delete;
    thread_pool_v3 &operator=(const thread_pool_v3 &) = delete;
    thread_pool_v3(thread_pool_v3 &&) = default;
    thread_pool_v3 &operator=(thread_pool_v3 &&) = default;
    // default move constructor and move assignment are fine

    template <typename F, typename Ret = decltype(std::declval<F>()())>
    std::future<Ret> add_task(F &&f)
    {
        std::packaged_task<Ret()> task(std::forward<F>(f));
        std::future<Ret> ret = task.get_future();
        
        if(local_tasks){
            auto share_ptr = std::make_shared<FuncContainer>(std::move(task));
            local_tasks->push(std::move(share_ptr));
            return ret;
        }
        tasks.push(FuncContainer(std::move(task)));
        return ret;
    }
    void run_task()
    {   std::shared_ptr<FuncContainer> task_ptr;
        if (local_tasks && !local_tasks->empty()){
            task_ptr = std::move(local_tasks->front());
            local_tasks->pop();
            (*task_ptr)();
        }
        else if (task_ptr = std::move(tasks.try_pop()))
        {
            (*task_ptr)();
        }
        else
        {
            std::this_thread::yield();
        }
    }
    ~thread_pool_v3()
    {
        done = true;
    }
};
//achieve another height!!! alomst 2 times faster than noraml future way!
//version 4：task_steal
//stealable queue for local queue
class stealable_queue{
    using FuncContainer = details::func_wrapper;
    std::deque<std::shared_ptr<FuncContainer>> tasks;
    std::mutex mutex;
    public:
    stealable_queue() = default;
    //delete copy constructor and copy assignment
    //but reserve move:
    stealable_queue(const stealable_queue&) = delete;
    stealable_queue& operator=(const stealable_queue&) = delete;
    stealable_queue(stealable_queue&&) = default;
    stealable_queue& operator=(stealable_queue&&) = default;
    //
    void push(std::shared_ptr<FuncContainer> task){
        std::lock_guard<std::mutex> lock(mutex);
        tasks.push_front(std::move(task)); // always process recently added task first
        //总是处理最近压入的任务，保证效率！ 可以预见，对于快速排序而言的最后一个函数 我们一定是去处理最先压入的
        //对应下面steal 从另一端夺取值
    }
    std::shared_ptr<FuncContainer> try_pop(){
        std::lock_guard<std::mutex> lock(mutex);
        if(tasks.empty()){
            return nullptr;
        }
        auto ret = std::move(tasks.front());
        tasks.pop_front();
        return ret;
    }
    std::shared_ptr<FuncContainer> try_steal(){
        std::lock_guard<std::mutex> lock(mutex);
        if(tasks.empty()){
            return nullptr;
        }
        auto ret = std::move(tasks.back());
        tasks.pop_back();
        return ret;
    }
    bool empty(){
        std::lock_guard<std::mutex> lock(mutex);
        return tasks.empty();
    }

};
class thread_pool_v4
{
private:
    // mention the order! threads need to be the last.
    using FuncContainer = details::func_wrapper;
    std::atomic<bool> done;
    normal_queue<FuncContainer> tasks;//global
    using local_queue = stealable_queue;
    inline static thread_local local_queue* local_tasks ;//to decrease the size,we use ptr.
    inline static thread_local int local_tag; 
    std::vector<std::unique_ptr<stealable_queue>>ptrs_local_tasks;
    std::vector<join_thread> threads;
    bool try_local_run(){
        std::shared_ptr<FuncContainer> task_ptr = nullptr;
        if(local_tasks &&(task_ptr = local_tasks->try_pop())){
                (*task_ptr)();
                return true;
        }
        return false;
    }
    bool try_global_run(){ // as i test if we transfer variable it will be slower
        std::shared_ptr<FuncContainer> task_ptr = nullptr;
        if(task_ptr = tasks.try_pop()){
            (*task_ptr)();
            return true;
        }
        return false;
    }
    bool try_steal_run(){
        std::shared_ptr<FuncContainer> task_ptr = nullptr;
        for(int i=0;i<ptrs_local_tasks.size();i++){//local_tag + size -1 + 1` 
            int now_index = (local_tag+i+1)%ptrs_local_tasks.size();//不要呆着一个薅羊毛  
            if((task_ptr = ptrs_local_tasks[now_index]->try_steal())){
                (*task_ptr)();
                return true;
            }
        }
        return false;
    }
    void work_thread()
    {
        while (!done)
        {
            run_task();
        }
    }

public:
    thread_pool_v4(int num_threads) : done(false), threads(num_threads)
    {
        // we use join thread. so we dont need to do like in the book
        for(int i=0;i<num_threads;i++){
              ptrs_local_tasks.emplace_back(new local_queue());
        }
        for (int i = 0; i < num_threads; i++)
        {   
            //i think if we use unique_ptr the momory safe can be assured
          
            //add initialization of the localqueue
            threads[i] = join_thread([this,i]() 
                                     {local_tag =i;
                                    local_tasks = ptrs_local_tasks[i].get();
                                     work_thread(); });
        }
    }
    thread_pool_v4(const thread_pool_v4 &) = delete;
    thread_pool_v4 &operator=(const thread_pool_v4 &) = delete;
    thread_pool_v4(thread_pool_v4 &&) = default;
    thread_pool_v4 &operator=(thread_pool_v4 &&) = default;
    // default move constructor and move assignment are fine

    template <typename F, typename Ret = decltype(std::declval<F>()())>
    std::future<Ret> add_task(F &&f)
    {
        std::packaged_task<Ret()> task(std::forward<F>(f));
        std::future<Ret> ret = task.get_future();
        
        if(local_tasks){
            auto share_ptr = std::make_shared<FuncContainer>(std::move(task));
            local_tasks->push(std::move(share_ptr));
            return ret;
        }
        tasks.push(FuncContainer(std::move(task)));
        return ret;
    }
    void run_task()
    {   
        if(try_local_run()||try_global_run()||try_steal_run()){
            return;
        }
        else{
            std::this_thread::yield();
        }
    }
    ~thread_pool_v4()
    {
        done = true;
    }
};