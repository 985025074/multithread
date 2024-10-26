#pragma once
#include "buffer.h"
template <typename T>
concept is_zhengshu = std::is_same_v<T, int> || std::is_same_v<T, long> || std::is_same_v<T, long long> || std::is_same_v<T, unsigned int> || std::is_same_v<T, unsigned long> || std::is_same_v<T, unsigned long long> ;
namespace syc{
    class LogStream
    {
    private:
        using Buffer = syc::details::Buffer<syc::details::defaut_buffer_size>;
        Buffer _buffer;

    public:
        LogStream() = default;
        ~LogStream() = default;
        LogStream(const LogStream &) = delete;
        LogStream &operator=(const LogStream &) = delete;
        ;
        // 整数打印
        template <typename T>
        requires is_zhengshu<T>
        LogStream &operator<<(T v)
        {
            _buffer.push_number(v);
            return *this;
        }
        LogStream &operator<<(const void *);

        LogStream &operator<<(float v)
        {
            *this << static_cast<double>(v);
            return *this;
        }
        LogStream &operator<<(double);

        LogStream &operator<<(char v)
        {
            _buffer.append(&v, 1);
            return *this;
        }

        LogStream &operator<<(const char *str)
        {
            if (str)
            {
                _buffer.append(str, strlen(str));
            }
            else
            {
                _buffer.append("(null)", 6);
            }
            return *this;
        }

        LogStream &operator<<(const unsigned char *str)
        {
            return operator<<(reinterpret_cast<const char *>(str));
        }

        LogStream &operator<<(const std::string &v)
        {
            _buffer.append(v.c_str(), v.size());
            return *this;
        }

        LogStream &operator<<(const std::string_view &v)
        {
            _buffer.append(v.data(), v.size());
            return *this;
        }

        LogStream &operator<<(const Buffer &v)
        {
            *this << v.toString();
            return *this;
        }
        unsigned int length() const { return _buffer.length();}
        const Buffer &buffer() const { return _buffer; }
        void resetBuffer() { _buffer.reset(); }

    };
}
