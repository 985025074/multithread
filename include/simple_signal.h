#pragma once
#include "std.h"
namespace syc{
template <typename Ret,typename ...Args>
class signal{
public:
    void register_slot(std::function<Ret(Args...)> slot){
        std::lock_guard<std::mutex> lock(mtx);
        slots.push_back(std::move(slot));
    }
    void unregister_slot(std::function<Ret(Args...)> slot){
        std::lock_guard<std::mutex> lock(mtx);
        slots.erase(std::find(slots.begin(),slots.end(),slot));
    }
    void call(Args... args){
        std::lock_guard<std::mutex> lock(mtx);
        for(auto& slot:slots){
            slot(args...);
        }
    }

private:
    std::mutex mtx;
    std::vector<std::function<Ret(Args...)>> slots;
};
};