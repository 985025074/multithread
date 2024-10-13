#pragma once
#include "std.h"
template <typename T>
class threadsafe_stack{
    private:
    mutable std::mutex mtx;
    std::stack<T> stack;
    public:
    threadsafe_stack() = default;
    threadsafe_stack(const threadsafe_stack& other){
        std::lock_guard<std::mutex> lock(mtx);
        stack = other.stack;
    }
    threadsafe_stack& operator=(const threadsafe_stack&) = delete;

    void push(const T& value){
        std::lock_guard<std::mutex> lock(mtx);
        stack.push(value);
    }
    bool empty() const{
        std::lock_guard<std::mutex> lock(mtx);
        return stack.empty();
    }
    std::shared_ptr<T> pop(){
        std::lock_guard<std::mutex> lock(mtx);
        if(stack.empty()){
            throw std::runtime_error("pop from empty stack");
        }
       auto ptr = std::make_shared<T>(std::move(stack.top()));
       stack.pop();
       return ptr;
    }
    void pop(T& value){
        std::lock_guard<std::mutex> lock(mtx);
        if(stack.empty()){
            throw std::runtime_error("pop from empty stack");
        }
        value = std::move(stack.top());
        stack.pop();
    }
};