#pragma once
#include "std.h"
//C++230引入atomic shared_ptr 才可用
//原书的实现方法太过繁琐略了。

#if __cplusplus >=202002L
template <typename T>
class lock_free_stack_shared {
private:
    struct node{
        std::shared_ptr<T>data;
        std::shared_ptr<node>next;
        node(T data,std::shared_ptr<node>next):data(std::make_shared<T>(std::move(data))),next(std::move(next)){};
    };
    std::atomic<std::shared_ptr<node>>head;
public:
    void push(T data){
        std::shared_ptr<node>new_node = std::make_shared<node>(data,nullptr);
        while(!head.compare_exchange_weak(new_node->next,new_node));
    }   
    std::shared_ptr<T> pop(){
        std::shared_ptr<node>old_head = head.load();//取出 然后弹掉
        while(old_head&&!head.compare_exchange_weak(old_head,old_head->next));
        return old_head?old_head->data:nullptr;
    }
};

#endif