#include "FileWrapper.h"
#include <sys/stat.h>
namespace syc
{   
    int parse_args(std::string_view args){
        int data = 0;
        for(char c : args){
            switch(c){
                case 'a':
                    data |= O_APPEND;
                    break;
                case 'c':
                    data |= O_CREAT;
                    break;
                case 'e':
                    data |= O_EXCL;
                    break;
                case 'n':
                    data |= O_NONBLOCK;
                    break;
                case 'r':
                    data |= O_RDONLY;
                    break;
                case 'w':
                    data |= O_WRONLY;
                    break;
                case 'x':
                    data |= O_EXCL;
                    break;
                default:
                    break;
            }
        }
        return data;
    }
    /// @brief open filename by fopen.  possible throw error.
    /// @param filename which you want to open
    AppendFile::AppendFile(std::string_view filename) noexcept(false):_already_write(0){
        _fp = fopen(filename.data(), "ae");//E代表cloxecel 保证自动关闭 在exceve执行后
        if(_fp == nullptr){
            perror("AppendFile");
            throw std::runtime_error("AppendFile open failed");
        }
        setbuffer(_fp, _buffer, sizeof(_buffer));//全缓冲
    }
    /// @brief add data to the end of the file,use fwrite_unlocked. it is possible failed! 
    /// if failed,we just write part of the data.you can use get_already_written to check.
    /// @param data input data
    void AppendFile::append(std::string_view data){
        int remain = data.size();
        while(remain > 0){
            int n = fwrite_unlocked(data.data() + data.size() - remain, 1, remain, _fp);
            if(ferror(_fp)){
                perror("AppendFile write");
                break;
                // throw std::runtime_error("AppendFile write failed");
            }
            remain -= n;
        }
        _already_write += data.size()-remain;
    }
    void AppendFile::flush(){
        fflush(_fp);
    }
    AppendFile::~AppendFile(){
        if(_fp!= nullptr)
            fclose(_fp);
    }
    ReadSmallFile::ReadSmallFile(std::string_view filename) noexcept(false){
        _fd = open(filename.data(), O_RDONLY|O_CLOEXEC);
        if(_fd < 0){
            _err = errno;
            perror("ReadSmallFile");
            fflush(stdout);
            throw std::runtime_error("ReadSmallFile open failed");
        }
    }
    ReadSmallFile::~ReadSmallFile(){
        if(_fd >= 0){
            close(_fd);
        }
    }
    /// @brief read data from file to string.
    /// CAUTION:YOU NEED ENSURE IT IS A FILE,NOT DIR
    /// @param max_size max size of data to read.
    /// @return ReadFileResult struct
    ///@note 注意 这里我们会把数据读入buffer 缓冲区，从0开始覆盖
    std::optional<ReadFileResult> ReadSmallFile::readToString(int max_size){
        struct stat st;
        ReadFileResult result;
        //here i dismiss check if it is a directory
        if(fstat(_fd, &st) == 0){
            result.file_size = st.st_size;
            result.content.reserve(std::min(static_cast<int64_t>(max_size), result.file_size));
            result.modify_time = st.st_mtime;
            result.create_time = st.st_ctime;
        }
        else{
            perror("ReadSmallFile fstat");
            _err = errno;
            return std::nullopt;
        }
        int remain = result.content.capacity();
        int written = 0;
        while(written < remain){
            int n = ::read(_fd, _buffer + written, remain);
            if(n < 0){
                _err = errno;
                perror("ReadSmallFile read");
                break;
            }
            result.content.append(std::string_view(_buffer, n));
            written += n;
        }
        return result;
    }
    /// @brief read once to buffer.
    /// USE PREAD which will not change file offset.
    /// @return size the number of bytes to read.
    ///@note 这里把数据读入缓冲区 从0处覆盖。
    int ReadSmallFile::readToBuffer(){
        int n = pread(_fd, _buffer, sizeof(_buffer)-1, 0);
        ssize_t size = n;
        if(n < 0){
            _err = errno;
            size = -1;
        }
        else{
            _buffer[n] = '\0';
            //leave a space for '\0'
        }
        return size;
    }
    std::optional<ReadFileResult> readFile(std::string&filename,int maxsize){
        ReadSmallFile file(filename);
        return file.readToString(maxsize);
    };
}; // namespace syc
