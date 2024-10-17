#pragma once
#include "std.h"
#include "timer.h"
#define LINE std::cout <<"--------------------------"<<std::endl;
template <typename Container>
void print_container(Container const& c) {
    for (auto const& e : c) {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}

class join_thread{
public:
    join_thread() = default;
    template <typename F, typename... Args,typename = std::enable_if_t<std::is_invocable_v<F,Args...>>>
    join_thread(F&& f, Args&&... args):t(std::forward<F>(f), std::forward<Args>(args)...){

    }
    join_thread(join_thread&&) = default;
    join_thread& operator=(join_thread&&) = default;

    void join(){
        if(t.joinable()){
        t.join();
        }
    }
    void detach(){
        if(t.joinable()){
        t.detach();
        }
    }
    ~join_thread(){
        if(t.joinable()){
            t.join();
        }
    }
private:
    std::thread t;
};