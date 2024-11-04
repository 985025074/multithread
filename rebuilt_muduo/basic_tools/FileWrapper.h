#pragma once
#include <string>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <optional>
#include <memory>
#include <chrono>
#define APPEND_FILE_BUFFER_SIZE 64*1024
#define READ_FILE_BUFFER_SIZE 64*1024
//提供了两个 fwrite 和 append 暂时不清楚
namespace syc{
    //appendfile 提供文件的加入 使用的是标准流
    
    /// @brief appendfile class 可能会创建文件，默认在filename 的后面追加
    /// 如果文件不存在，构造函数抛出异常
    class AppendFile{
        public:
        //delete copy constructor and copy assignment
        AppendFile(const AppendFile&) = delete;
        AppendFile& operator=(const AppendFile&) = delete;
        AppendFile(AppendFile&&) = default;
        AppendFile& operator=(AppendFile&&) = default;
        //
        explicit AppendFile(std::string_view filename);
        ~AppendFile();
        void append(std::string_view data);
        void flush();
        off_t get_already_write(){
            return _already_write;
        }
        private:
        FILE* _fp;
        char _buffer[APPEND_FILE_BUFFER_SIZE];
        off_t _already_write;
    };
    //小文件处理
    struct ReadFileResult{
        std::string content;
        int64_t file_size;
        int64_t create_time;
        int64_t modify_time;
    };
    class ReadSmallFile{
        public:
        ReadSmallFile(std::string_view filename);
        std::optional<ReadFileResult> readToString(int maxsize);
        int readToBuffer();
        ~ReadSmallFile();
        const char *buffer() const{
            return _buffer;
        }
        int get_err() const{
            return _err;
        }
        private:
        int _fd;
        int _err;
        char _buffer[READ_FILE_BUFFER_SIZE];
    };
    
std::optional<ReadFileResult> readFile(std::string&filename,int maxsize);
};
