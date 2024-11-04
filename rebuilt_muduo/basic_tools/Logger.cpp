#include "Logger.h"
#include <unistd.h>
#include <sys/uio.h>
#include "FormatTools.h"
namespace syc
{   
     const char* const map_to_log_level[Logger::NUMBER_OF_LOG_LEVELS] ={
        "TRACE",
        "DEBUG",
        "INFO",
        "WARN",
        "ERROR",
        "FATAL"
    };
    Logger::Logger(NowFile file, int line,std::string time,int level):
        _impl{.file = syc::details::get_filename(file).data(),.line = line,.time = time ,.level = level}{
            _impl._stream<<time<<_impl.file<<" "<<line <<" "<<map_to_log_level[level]<<" : ";
    };
    //初始化
    int Logger::LogLevel = INFO;
    



    void default_callback(Logger::impl &&impl){
         fwrite(impl._stream.buffer().data(),1 ,impl._stream.length(),stdout);
    }
    void default_flush(){
        fflush(stdout);
    }
    Logger::GlobalCallback global_call = &default_callback;
    Logger::GlobalFlush global_flush = &default_flush;
    void Logger::setGlobalCallback(Logger::GlobalCallback callback){
        global_call = callback;
    }
    Logger::~Logger(){
        if(_impl.level == FATAL){
            global_call(std::move(_impl));
           global_flush();
           abort();
           return; 

        }

        global_call(std::move(_impl));
    }
    
};