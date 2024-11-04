#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include "FileWrapper.h"
#include "helper.h"
//与具体系统 绝对时间交互还是用std::time
namespace syc
{
    /// @brief 提供最基本的读写文件类 在append file 基础上加入滚动功能
     using namespace std::literals;
    class LogFile
    {
        
        // delete copy constructor and assignment operator
        LogFile(const LogFile &) = delete;
        LogFile &operator=(const LogFile &) = delete;

    public:
       
        // 参数是什么意思
        LogFile(const std::string &basename, off_t rollsize, bool threadsafe = false, time_t flushInterval = 3, int checkEveryN = 1024);
        void append_unlocked(std::string_view);
        void append(std::string_view);
        void flush();
        void rollfile(time_t*getTime = nullptr); // 滚动的含义是创建一个新的文件，根据当前时间
        std::string get_log_file_name(time_t*getTime = nullptr);
    private:
        std::string _basename;
        std::mutex _mtx;
        bool _threadsafe;
        off_t _rollsize;
        int _checkEveryN;
        int _count;
        time_t _flushInterval;
        const static time_t _rollInterval = 60*60*24; //注意单位全秒
        time_t _last_roll_time;
        time_t _last_flush_time;
        std::unique_ptr<syc::AppendFile> _file;
        
        long files_count;
    };
}