#include "LogStream.h"
#include <format>

using syc::LogStream;




LogStream& LogStream::operator<<(const void* ptr){
    *this << std::format("{:p}", ptr);
    return *this;
}
LogStream& LogStream::operator<<(double v){
    *this << std::format("{:.12f}", v);
    return *this;
}
