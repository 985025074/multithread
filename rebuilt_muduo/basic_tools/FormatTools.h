#pragma once
#include <format>
#include <iostream>
#include <ctime>
namespace syc{
namespace details{
std::string_view get_filename(std::string_view filename);
std::string get_gmt_time(time_t *getTime = nullptr);
};
};