#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include "FileWrapper.h"
namespace syc
{
    /// @brief 提供最基本的读写文件类 在append file 基础上加入滚动功能
    class LogFile
    {
        // delete copy constructor and assignment operator
        LogFile(const LogFile &) = delete;
        LogFile &operator=(const LogFile &) = delete;

    public:
        // 参数是什么意思
        LogFile(const std::string &basename, off_t rollsize, bool threadsafe = false, int flushInterval = 1, int checkEveryN = 1024);
        void append(std::string_view);
        void flush();
        void rollfile(); // 滚动的含义是创建一个新的文件，根据当前时间
        std::string get_log_file_name();


    private:
        std::string _basename;
        std::mutex _mtx;
        int _flushInterval;
        int _checkEveryN;
        std::unique_ptr<syc::AppendFile> _file;
    };
}