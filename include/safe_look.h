#pragma once
#include "std.h"
#include "timer.h"
// 为了并发 不选择树 因为树要获取根节点 相当复杂
// 读锁 写锁获取时机

template <typename Key, typename Value, typename Hash = std::hash<Key>>
class threadsafe_map;
namespace details
{

    template <typename Key, typename Value>
    class bucket
    {   
    template <typename , typename , typename >
    friend class ::threadsafe_map;

    public:
        using ValuePair = std::pair<Key, Value>;
        using Pairs = std::list<ValuePair>;
        using bucket_iterator = typename Pairs::iterator;
        bucket() = delete; // 至少应该有Num值的指定
        bucket(size_t num) : _num(num) {}
        bucket(const bucket &other) = delete;
        bucket &operator=(const bucket &other) = delete;
        bucket(bucket &&other) : _pairs(std::move(other._pairs)), _num(other._num)
        {
        }
        auto find_first(const Key &key) const
        {
            std::shared_lock<std::shared_mutex> lock(mtx);
            bucket_iterator loc = find_first_impl(key);
            return loc;
        }
        bucket_iterator find_first(const Key &key)
        {
            std::shared_lock<std::shared_mutex> lock(mtx);
            bucket_iterator loc = find_first_impl(key);
            return loc;
        }
        void insert_value(const Key &key, const Value &value)
        {
            std::lock_guard<std::shared_mutex> lock(mtx);
            auto loc = find_first_impl(key);
            if (loc == _pairs.end())
            {

                _pairs.emplace_back(key, value);
            }
            else
            {
                loc->second = value;
            }
        }
        bool remove_value(const Key &key)
        {
            std::lock_guard<std::shared_mutex> lock(mtx);
            auto loc = find_first_impl(key);
            if (loc == _pairs.end())
            {
                return false;
            }
            else
            {
                _pairs.erase(loc);
                return true;
            }
        }
        bool check_num(const size_t &query) const
        {
            return _num == query;
        }
        auto get_lock()const
        {
            return std::move(std::unique_lock<std::shared_mutex>(mtx));
        }

    private:
        // trick dont be constant or it will be constant
        bucket_iterator find_first_impl(const Key &key)
        {
            return std::find_if(_pairs.begin(), _pairs.end(), [&key](const ValuePair &pair)
                                { return key == pair.first; });
        }
        auto find_first_impl(const Key &key) const
        {
            return std::find_if(_pairs.begin(), _pairs.end(), [&key](const ValuePair &pair)
                                { return key == pair.first; });
        }
        Pairs _pairs;
        const size_t _num;
        mutable std::shared_mutex mtx;
    };

};
template <typename Key, typename Value, typename Hash >
class threadsafe_map
{
public:
    using Bucket = details::bucket<Key, Value>;
    using Buckets = std::vector<Bucket>;
    threadsafe_map(size_t bucket_count = 19, Hash &&hasher = std::hash<Key>()) : hasher(std::move(hasher))
    {
        _buckets.reserve(bucket_count);
        for (size_t i = 0; i < bucket_count; i++)
        {
            _buckets.emplace_back(Bucket(i));
        }
    }
    size_t calc_num(const Key &key) const
    {
        return hasher(key) % _buckets.size();
    }
    Value find_value(const Key &key)
    {
        auto num = calc_num(key);
        auto iter = _buckets[num].find_first(key);
        return iter->second;
    }
    void insert_value(const Key &key, const Value &value)
    {
        auto num = calc_num(key);
        _buckets[num].insert_value(key, value);
    }
    bool remove_value(const Key &key)
    {
        auto num = calc_num(key);
        return _buckets[num].remove_value(key);
    }
    auto getMap() const
    {
        std::vector<std::unique_lock<std::shared_mutex>> locks;
        locks.reserve(_buckets.size());
        for (auto &bucket : _buckets)
        {
            locks.emplace_back(bucket.get_lock());
        }
        std::map<Key, Value> result;
        for (auto &bucket : _buckets)
        {
            for (auto &pair : bucket._pairs)
                result[pair.first] = pair.second;
        }
        return result;
    }

private:
    Buckets _buckets;
    std::function<size_t(const Key &)> hasher;
};

namespace normal{
template <typename Key, typename Value>
class threadsafe_map
{
public:
    threadsafe_map() = default;
    Value find_value(const Key &key)
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto loc = _map.find(key);
        if(loc!= _map.end())
            return loc->second;
        else
            return Value();
    }
    void insert_value(const Key &key, const Value &value)
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto loc = _map.find(key);
        if(loc!= _map.end())
            loc->second = value;
        else
            _map.insert(std::make_pair(key, value));
    }
private:
    std::map<Key, Value> _map;
    std::mutex mtx;
};
};
namespace fortest_look{
template <typename container>
void create(int start,int end,container& q){
    for(int i =start;i<=end;i++){
       q.insert_value(i,i);
    }
}
template <typename container>
void del(int num,container& q){
    for(int i =0;i<=num;i++){
       q.find_value(i);
    }
}
template <typename container>
void benchmark(int times,container& p){
    int64_t result =0;
    
    auto now = syc::Timer();
    for(int i =0;i<times;i++){
    std::vector<std::thread>pool;
    pool.emplace_back(create<container>,0,10000,std::ref(p));
    pool.emplace_back(create<container>,10001,20000,std::ref(p));
    pool.emplace_back(del<container>,20000,std::ref(p));
    
    for(auto& t:pool){
        t.join();
    }
        result+= now.stop();
    }
    result/=times;
    std::cout << "benchmark result:" << result << std::endl;
}
};