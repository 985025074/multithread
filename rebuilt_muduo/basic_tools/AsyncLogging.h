// 异步日志 大约思路：
// 我们维护2个缓冲区池
// 一个是在写的的缓冲区池，一个是去写的缓冲区

// 前台线程 获取两个变量，当前在写，下一个要写。
//
// 前台线程：append.
// 负责写数据,暂存到缓冲区
// 如果满了，加入队列，通知后台线程 cond 变量通知

// 后台线程：
// 负责logfile
//  while(外部控制的一个变量)
//  if(当前在写的线程池为空){
//  等，等多久？等刷新时间}
// 交换顺序，把写的线程池子 换到 去写的线程池子 swap
//  如果堆积过多 超过25个 扔掉多余的 只保留两个，控制台打印相关信息。
//
// for 循环遍历每个buffer
// 在logfile 中 append
#pragma once
#include "buffer.h"
#include "Logger.h"
#include "LogFile.h"

#include <vector>
#include <memory>
namespace syc
{
    class AsyncLogging
    {
        public:
        AsyncLogging(std::string basename) : _basename(basename)
        {
            _thread__finished_future = _thread_finished.get_future().share();
            _frontPool.reserve(2);
            _frontPool.emplace_back(std::make_unique<Buffer>());
            _frontPool.emplace_back(std::make_unique<Buffer>());
            cur_write_buffer = _frontPool[0].get();
            next_write_buffer = _frontPool[1].get();
        }
        void start()
        {
            _running = true;
            backend = join_thread([this](){thread_write();});
        }
        void stop()
        {
            _running = false;
            _cond_write.notify_one();
            backend.join();
        }
        void append(std::string_view data)
        {
            _thread__finished_future.wait();
            std::lock_guard<std::mutex> lgd(_front_pool_lock);
            assert(_frontPool.size() >= 2);
            if (cur_write_buffer->avail() >= data.size())
            { // 当前有空，写吧
                cur_write_buffer->append(data.data(), data.size());
                return;
            }
            else if (next_write_buffer)
            { // 没空，看看我预留的如何
                cur_write_buffer = next_write_buffer;
                next_write_buffer = nullptr;
            }
            else
            { // 要新产生一个了 没办法
                _frontPool.emplace_back(std::make_unique<Buffer>());
                cur_write_buffer = _frontPool.back().get();
            }
            cur_write_buffer->append(data.data(), data.size());
        }
        void thread_write();
    private:
        std::string _basename; // 日志前缀
        std::atomic<bool> _running;
        std::promise<void> _thread_finished;
        std::shared_future<void> _thread__finished_future;
        constexpr static inline int flush_interval = 10;      // 刷新时间 1s;
        constexpr static inline int roll_size = 500*1000*1000;        // 日志文件大小 4KB;
        constexpr static inline int max_num_trunk = 2;       
        using Buffer = details::Buffer<roll_size>; // 暂定
        using BufferPool = std::vector<std::unique_ptr<Buffer>>;
        // 约束 保护前端池子
        std::mutex _front_pool_lock;
        std::condition_variable _cond_write;
        BufferPool _frontPool;
        BufferPool _backPool;
        Buffer *cur_write_buffer;
        Buffer *next_write_buffer;
        //
        join_thread backend;
    };
};