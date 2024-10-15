#pragma once
#include "std.h"
template <typename Key,typename Value>
requires requires(Value v) {
    requires std::is_same_v<std::decay_t<decltype(std::declval<Value>().key())>,Key>;
    {Value(v.key())};
}
class classMap : public std::enable_shared_from_this<classMap<Key, Value>>{
    public:
    using std::enable_shared_from_this<classMap<Key, Value>>::shared_from_this;//模板类必须要前向声明不然报错
    std::shared_ptr<Value> get(const Key& key){
        std::lock_guard<std::mutex> lock(_mutex);
        std::shared_ptr<Value>& result = _map[key];
        if(result == nullptr){
            std::weak_ptr<classMap<Key, Value>> this_ptr = shared_from_this();//循环引用！！！
            std::shared_ptr<Value> new_value(new Value(key),[this_ptr = std::move(this_ptr)](Value*ptr){
                if(!this_ptr.expired()){
                    auto temp = this_ptr.lock();
                    std::lock_guard<std::mutex> lock(temp->_mutex);
                    temp->_map.erase(ptr->key());
                }
                delete ptr;
            });   
            
            result = std::move(new_value);
        }
        return result;
    }
    private:
    std::map<Key,std::shared_ptr<Value>> _map;
    mutable std::mutex _mutex;
};