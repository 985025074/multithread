#include "LogFile.h"
#include "helper.h"
#include "FormatTools.h"
#include <iostream>
#define WRITEABLE 0644
namespace syc{
    LogFile::LogFile(const std::string &basename, off_t rollsize, bool threadsafe , time_t flushInterval, int checkEveryN )
    :_basename(basename),_rollsize(rollsize),_threadsafe(threadsafe),_flushInterval(flushInterval),_checkEveryN(checkEveryN)
    {
        time_t now;
        rollfile(&now);
        _last_flush_time = now;
        _last_roll_time = now;
    }
    //根据一时间，线程名 创建一个合适的日志名
    std::string LogFile::get_log_file_name(time_t*getTime){
        std::string a(details::get_gmt_time(getTime));
        auto _ =split_by(a,' ');
        return std::format("{}-{}-{}-{}-{}-{}.log",_basename,gettid(),_[5],_[4],_[3],files_count++);
    }
    //作用是 新建一个日志文件
    void LogFile::rollfile(time_t*getTime){
        //这里可以进一步优化，将时间从get_fmt_time 获得，减少了一次系统调用，我这里图省事略过了
        std::string new_file_name =  get_log_file_name(getTime);
        
        _file = std::make_unique<AppendFile>(new_file_name.c_str());
        std::cout << "rollfile" << std::endl;
    }
    //这里需要加锁！ 
    void LogFile::flush(){
        std::cout << "flush" << std::endl;
        if(_threadsafe){
            std::lock_guard<std::mutex> lock(_mtx);
            _file->flush();
        }
        else{
            _file->flush();
        }
    }
    //append 大致思路
    //threadsafe? 拿锁与不拿锁
    //判断文件是否过大？ rollsize,如果过大，roll
    //checkEveryN 的作用是每隔一段时间 检查。
    //检查内容包括 定时flush 定时roll
    //append 也需要加锁与不加锁衡量过！
    void LogFile::append_unlocked(std::string_view data){
        auto now = std::time(nullptr);
        if(_file->get_already_write()>_rollsize){
            rollfile();
        }
        else{
            _count++;
            if(_count == _checkEveryN){
                _count =0;
                //check_flush();
                if(now - _last_flush_time > _flushInterval){
                    flush();
                    _last_flush_time = now;
                }
                else if (now - _last_roll_time > _rollInterval){
                    rollfile();
                    _last_roll_time = now;
                    _last_flush_time = now;//remember update this.
                }
            }
        }
        _file->append(data);
    }
    void LogFile::append(std::string_view data){
        if(_threadsafe){
            std::lock_guard<std::mutex> lock(_mtx);
            append_unlocked(data);
        }
        else{
            append_unlocked(data);
        }
    }
};