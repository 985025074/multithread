#include "std.h"
template<typename T>
class threadsafe_list{
    private:
    struct node{
        std::mutex lock;
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
        node(T data):data(std::make_shared<T>(std::move(data))){

        }
    };
    std::unique_ptr<node> head;//头节点始终为空
    public:
    threadsafe_list():head(std::make_unique<node>(T())){

    }
    threadsafe_list(const threadsafe_list& other) = delete;
    threadsafe_list& operator=(const threadsafe_list& other) = delete;
    threadsafe_list(threadsafe_list&& other) = default;
    threadsafe_list& operator=(threadsafe_list&& other) = default;
    void push_front(T data){
        auto new_node = std::make_unique<node>(std::move(data));
        std::lock_guard<std::mutex> lock(head->lock);//拿下头节点的锁
        new_node->next = std::move(head->next);
        head->next = std::move(new_node);
    }
    template <typename F>
    void for_each(const F &f)const{
        auto current = head.get();
        std::unique_lock<std::mutex> lock(current->lock);
        while(auto temp =current->next.get()){
            //暂存下current->next可以优化 更快释放掉current的锁
            std::unique_lock<std::mutex> next_lock(current->next->lock);
            lock.unlock();
            f(*current->next->data);
            current = temp;
            lock = std::move(next_lock);
        }
    }
    template <typename F>
    std::shared_ptr<T> find_first(const F& f) const{
        auto current = head.get();
        std::unique_lock<std::mutex> lock(current->lock);
        while(auto temp =current->next.get()){
            //暂存下current->next可以优化 更快释放掉current的锁
            std::unique_lock<std::mutex> next_lock(current->next->lock);
            lock.unlock();
            if(f(*current->next->data)){
                return current->next->data;
            }
            current = temp;
            lock = std::move(next_lock);
        }
        return nullptr;
    }
    template <typename F>
    bool remove_if(const F& f){
        auto current = head.get();
        std::unique_lock<std::mutex> lock(current->lock);
        while(node* temp=current->next.get()){
            //暂存下current->next可以优化 更快释放掉current的锁
            std::unique_lock<std::mutex> next_lock(current->next->lock);
            if(f(*current->next->data)){
                current->next = std::move(current->next->next);
            }
            else{
                lock.unlock();
                lock = std::move(next_lock);
                current = temp;
            }
        }
        return true;
    }
        
};