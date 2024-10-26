#pragma once
#include "std.h"
//改进为shared_ptr T 保证拷贝不会异常
template <typename T>
class threadsafe_queue{
private:
    std::mutex head_mtx;
    std::mutex tail_mtx;
    struct node{
        std::shared_ptr<T> data;
        std::unique_ptr<node>next;
        node(std::shared_ptr<T> data,std::unique_ptr<node> next):data(std::move(data)),next(std::move(next)){}
    };
    std::unique_ptr<node> head = nullptr;
    node*tail = nullptr;
    std::condition_variable data_ok;
public:
    threadsafe_queue():head(std::make_unique<node>(nullptr,nullptr)),tail(head.get()){
        //注意上面是一个虚位节点
    }
    threadsafe_queue(const threadsafe_queue&) = delete;
    threadsafe_queue& operator=(const threadsafe_queue&) = delete;
    threadsafe_queue(threadsafe_queue&&) = default;
    threadsafe_queue& operator=(threadsafe_queue&&) = default;
    //得益于unique_ptr管理 我们不需要写析构函数

    // node *getHead(){
    //     return head.get();
    // }
    // const node *getHead() const{
    //     return head.get();
    // }
    // node *getTail(){
    //     return tail;
    // }
    // const node *getTail() const{
    //     return tail;
    // }
    void push(T data){
        //在虚位节点情况下 向尾部插入元素 是这样的,直接在尾部构造对应元素.
        auto new_tail = std::make_unique<node>(std::make_shared<T>(std::move(data)),nullptr);
        auto new_tail_ptr = new_tail.get();
        {
            std::lock_guard<std::mutex> tail_lock(tail_mtx);
            tail->data = new_tail->data; 
            tail->next = std::move(new_tail);
            tail = new_tail_ptr;
        }
        data_ok.notify_one();
    }
    std::shared_ptr<T> try_pop(){
        node*tail_temp = nullptr;
        std::shared_ptr<T> result;
        {
            std::lock_guard<std::mutex> head_lock(head_mtx);
            {   
                std::lock_guard<std::mutex> tail_lock(tail_mtx);
                tail_temp = tail;
            }
            if(tail_temp == head.get()) return nullptr;
            result = std::move(head->data);
            head = std::move(head->next);
        }
        return result;
    }
    bool try_pop(T& value){
        std::lock_guard<std::mutex> head_lock(head_mtx);
        node* tail_temp = nullptr;
        {
            std::lock_guard<std::mutex> tail_lock(tail_mtx);
            tail_temp = tail;
        }
        if(tail_temp ==head.get()) return false;
        value = std::move(*head->data);
        head = std::move(head->next);
        return true;

    }
    std::unique_lock<std::mutex> wait_for_data(){
        std::unique_lock<std::mutex> lock(head_mtx);
        data_ok.wait(lock,[&]{
            {
                std::lock_guard<std::mutex> tail_lock(tail_mtx);
                return tail!=head.get();
            }
        });
        return lock;
    }
    std::shared_ptr<T> wait_and_pop(){
        std::shared_ptr<T> data;
        {std::unique_lock<std::mutex> lock(wait_for_data());
        //有数据
            data = std::move(head->data);
            head =std::move(head->next);
        }
        return data;
    }
   void wait_and_pop(T& value){
        std::unique_lock<std::mutex> lock(wait_for_data());
        value = std::move(*head->data);
        head =std::move(head->next);
    }
    bool empty() const{
        std::lock_guard<std::mutex> head_lock(head_mtx);
        {
            std::lock_guard<std::mutex> tail_lock(tail_mtx);
            return head.get()==tail;
        }
    }

};
template <typename T>
class normal_queue{
    std::queue<T> q;
    std::mutex mtx;
    std::condition_variable cv; 
public:
    normal_queue() = default;
    normal_queue(const normal_queue&) = delete;
    normal_queue& operator=(const normal_queue&) = delete;
    normal_queue(normal_queue&&) = default;
    normal_queue& operator=(normal_queue&&) = default;
    void push(T data){
        std::lock_guard<std::mutex> lock(mtx);
        q.push(std::move(data));
        cv.notify_one();
    }
    std::shared_ptr<T> wait_and_pop(){
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock,[this]{return!q.empty();});
        auto result = std::make_shared<T>(std::move(q.front()));
        q.pop();
        return result;
    }
    std::shared_ptr<T> try_pop(){
        std::lock_guard<std::mutex> lock(mtx);
        if(q.empty()) return nullptr;
        auto result = std::make_shared<T>(std::move(q.front()));
        q.pop();
        return result;
    }
};

namespace books{

template<typename T>
class threadsafe_queue
{
private:
    struct node
    {
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
    };
    
    std::mutex head_mutex;
    std::unique_ptr<node> head;
    std::mutex tail_mutex;
    node* tail;
    std::condition_variable data_cond;
public:
    threadsafe_queue():
        head(new node),tail(head.get())
    {}
    threadsafe_queue(const threadsafe_queue& other)=delete;
    threadsafe_queue& operator=(const threadsafe_queue& other)=delete;
    void push(T new_value)
    {
        std::shared_ptr<T> new_data(
            std::make_shared<T>(std::move(new_value)));
        std::unique_ptr<node> p(new node);
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data=new_data;
            node* const new_tail=p.get();
            tail->next=std::move(p);
            tail=new_tail;
        }
        data_cond.notify_one();
    }
private:
    node* get_tail()
    {
        std::lock_guard<std::mutex> tail_lock(tail_mutex);
        return tail;
    }

    std::unique_ptr<node> pop_head()
    {
        std::unique_ptr<node>  old_head=std::move(head);
        head=std::move(old_head->next);
        return old_head;
    }

    std::unique_lock<std::mutex> wait_for_data()
    {
        std::unique_lock<std::mutex> head_lock(head_mutex);
        data_cond.wait(head_lock,[&]{return head.get()!=get_tail();});
        return std::move(head_lock);
    }

    std::unique_ptr<node> wait_pop_head()
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        return pop_head();
    }

    std::unique_ptr<node> wait_pop_head(T& value)
    {
        std::unique_lock<std::mutex> head_lock(wait_for_data());
        value=std::move(*head->data);
        return pop_head();
    }
        
public:
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_ptr<node> const old_head=wait_pop_head();
        return old_head->data;
    }

    void wait_and_pop(T& value)
    {
        std::unique_ptr<node> const old_head=wait_pop_head(value);
    }
private:
    std::unique_ptr<node> try_pop_head()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail())
        {
            return std::unique_ptr<node>();
        }
        return pop_head();
    }

    std::unique_ptr<node> try_pop_head(T& value)
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        if(head.get()==get_tail())
        {
            return std::unique_ptr<node>();
        }
        value=std::move(*head->data);
        return pop_head();
    }

public:
    std::shared_ptr<T> try_pop()
    {
        std::unique_ptr<node> const old_head=try_pop_head();
        return old_head?old_head->data:std::shared_ptr<T>();
    }

    bool try_pop(T& value)
    {
        std::unique_ptr<node> const old_head=try_pop_head(value);
        return old_head;
    }

    bool empty()
    {
        std::lock_guard<std::mutex> head_lock(head_mutex);
        return (head==get_tail());
    }

};



}