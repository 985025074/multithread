#pragma once

#include "LogStream.h"
#include "convert.h"
#include "FormatTools.cpp"
#include <unistd.h>
namespace syc{

class Logger {
 
    using NowFile = std::string_view;
    public:
    static int LogLevel;
    enum  log_level{
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUMBER_OF_LOG_LEVELS
    };
    Logger() = delete;
    Logger(NowFile file, int line,std::string time,int level);

    //wrap message here
    struct impl{
        LogStream _stream;
        NowFile file;
        int line;
        std::string time;
        int level;
    };
    impl _impl;
    using GlobalCallback = void (*)(syc::Logger::impl&& impl);
    using GlobalFlush = void (*)();
    static void setGlobalCallback(GlobalCallback);
    static void setGlobalFlush(GlobalFlush);

    ~Logger();
};

};
#define FILE_NAME syc::details::get_filename(__FILE__)
// #define FILE_NAME "test.cpp"
#define TIME_NOW syc::details::get_gmt_time()
// #define TIME_NOW "Sat Oct 26 14:33:03 2024"
#define SYC_LOG_TRACE if(syc::Logger::LogLevel<=syc::Logger::TRACE)\
syc::Logger(__FILE__,__LINE__,TIME_NOW,syc::Logger::TRACE)._impl._stream    
#define SYC_LOG_DEBUG if(syc::Logger::logLevel<=syc::Logger::DEBUG)\
syc::Logger(__FILE__,__LINE__,TIME_NOW,syc::Logger::DEBUG)._impl._stream    
#define SYC_LOG_INFO if(syc::Logger::logLevel<=syc::Logger::INFO)\
syc::Logger(__FILE__,__LINE__,TIME_NOW,syc::Logger::INFO)._impl._stream    
#define SYC_LOG_WARN \
syc::Logger(__FILE__,__LINE__,TIME_NOW,syc::Logger::WARN)._impl._stream    
#define SYC_LOG_ERROR \
syc::Logger(__FILE__,__LINE__,TIME_NOW,syc::Logger::ERROR)._impl._stream    
#define SYC_LOG_FATAL \
syc::Logger(__FILE__,__LINE__,TIME_NOW,syc::Logger::FATAL)._impl._stream    