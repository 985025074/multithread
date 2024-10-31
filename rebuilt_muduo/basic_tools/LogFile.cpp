#include "LogFile.h"
#include "helper.h"
#include "FormatTools.h"
#include <iostream>
namespace syc{
    LogFile::LogFile(const std::string &basename, off_t rollsize, bool threadsafe , int flushInterval , int checkEveryN ){
        // append_file = 
    }
    std::string LogFile::get_log_file_name(){
        std::string a(details::get_gmt_time());
        auto _ =split_by(a,' ');
        return std::format("{}-{}-{}-{}-{}.log",_basename,gettid(),_[4],_[3],_[2]);
    }
};