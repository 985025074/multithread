#pragma once
#include "std.h"
namespace syc{
struct Timer{
    std::chrono::steady_clock::time_point last_point;
    Timer(){
        last_point = std::chrono::steady_clock::now();
    }
    auto stop(){
        auto now= std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_point).count();
        last_point = now;
        return duration;
    }
};
}