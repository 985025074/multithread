#include "AsyncLogging.h"
namespace syc
{

    void AsyncLogging::thread_write()
    {
        LogFile logfile(_basename, roll_size);
        _backPool.reserve(16); // 最多允许16个 暂定
        _backPool.emplace_back(std::make_unique<Buffer>());
        _backPool.emplace_back(std::make_unique<Buffer>());
        _thread_finished.set_value();
        while (_running)
        {
            
            {
                std::unique_lock<std::mutex> lgd(_front_pool_lock);
                do
                {
                    _cond_write.wait_for(lgd, std::chrono::seconds(flush_interval));
                } while (_frontPool.empty()&&_running);
                // wait until writebale!
                _frontPool.swap(_backPool);
                // 交换池子
            }
            for (auto &buffer : _backPool)
            {
                logfile.append(std::string_view(buffer->data(), buffer->length()));
            }
            // 清理完毕
            _backPool.resize(2); // 只保留2 个 以供前台写
            _backPool[0]->reset();
            _backPool[1]->reset();
            logfile.flush();
        }
        logfile.flush();
    }
};