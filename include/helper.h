#pragma once
#include "std.h"
#define LINE std::cout <<"--------------------------"<<std::endl;
template <typename Container>
void print_container(Container const& c) {
    for (auto const& e : c) {
        std::cout << e << " ";
    }
    std::cout << std::endl;
}