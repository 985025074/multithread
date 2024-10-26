#pragma once
#include <cstring>
#include <string>
#include <string_view>
#include "convert.h"
#include <iostream>
#include <boost/type_index.hpp>
using boost::typeindex::type_id_with_cvr;
#define TYPE(A) std::cout<<type_id_with_cvr<decltype(A)>().pretty_name()<<std::endl;
namespace syc
{
    namespace details
    {
        inline const int defaut_buffer_size = 4000;
        inline const int large_buffer_size = 8124;

        template <int SIZE>
        class Buffer
        {
        public:
            Buffer() = default;
            ~Buffer() = default;
            Buffer(const Buffer &) = delete;
            Buffer &operator=(const Buffer &) = delete;
            int avail()
            {
                return SIZE - _top;
            }
            /// @brief 返回原始指针
            /// @return 返回指针值
            char *data()
            {
                return _data;
            }
            const char *data() const
            {
                return _data;
            }
            
            char *current()//暂无常量版本
            {
                return _data + _top;
            }
            /// @brief 推入元素用户负责判断大小
            /// @param data 原始指针值
            /// @param len 长度值
            void append(const char *data, int len)
            {
                if (avail() > len)
                {
                    memcpy(_data + _top, data, len);
                    _top = _top + len;
                }
            }
            void reset()
            {
                _top = 0;
            }
            std::string_view toString() const
            {
                return std::string_view(_data, _top);
            }
            template <typename T>
            void push_number(T num)
            {
                if (avail() > allow_max_size_number)
                {
                    size_t size = convert(current(), num);

                    _top += size;
                }
            }
            unsigned int length() const{
                return _top;
            }

        private:
            char _data[SIZE];
            unsigned int _top = 0;                          // top 定义为真实元素 位置往上一格
            constexpr static int allow_max_size_number = 48; // 指示剩余多少允许推入数字
        };

    };

};
