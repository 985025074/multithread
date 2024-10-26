#pragma once
#include "std.h"

template <typename T>
class threadsafe_singleton {
public:
   static std::shared_ptr<T> getInstance() {
          std::call_once(inner_flag, [&]() {
              instance = std::make_shared<T>();});
          return instance;
   }
   private:
   inline static std::once_flag inner_flag;
   inline static std::shared_ptr<T> instance;

};