#pragma once
#include <format>
#include <iostream>
#include <ctime>
namespace syc{
namespace details{
std::string analyze_filename(std::string filename);
std::string get_gmt_time(time_t *getTime = nullptr);
};
};