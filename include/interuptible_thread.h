//
#pragma once
#include "helper.h"
#include "std.h"
using namespace std::literals;
struct interupt_flag{
    interupt_flag() = default;
    //禁止拷贝
    interupt_flag(const interupt_flag&) = delete;
    interupt_flag& operator=(const interupt_flag&) = delete;
    //禁止移动
    interupt_flag(interupt_flag&&) = delete;
    interupt_flag& operator=(interupt_flag&&) = delete;
    std::condition_variable*bind_cv;
    std::condition_variable_any*bind_cv_any;
    bool _flag = false;
    std::mutex _mutex;
    void add_condition_unsafe(std::condition_variable& cv){  
        bind_cv = &cv;
    }
    void add_condition(std::condition_variable& cv){ 
        std::lock_guard<std::mutex> lock(_mutex);
        bind_cv = &cv;
    }
    void add_condition_any(std::condition_variable_any& cv){ 
        std::lock_guard<std::mutex> lock(_mutex);
        bind_cv_any = &cv;
    }
    void del_condition(){
        std::lock_guard<std::mutex> lock(_mutex);
        bind_cv = nullptr;
        bind_cv_any = nullptr;
    }
    
    void set(){
        _flag = true;
        std::lock_guard<std::mutex> lock(_mutex);
        if(bind_cv){
            bind_cv->notify_all();
        }
        if(bind_cv_any){
            bind_cv_any->notify_all();
        }
    }
    bool is_set(){
        return _flag;
    }
    void clear(){
        _flag = false;
        del_condition();
    }
    ~interupt_flag(){
        //异常安全性 防止孔璇指针
        clear();
    }
};
thread_local interupt_flag interupt_flag_this;
class interuptible_thread{
    private:
    join_thread t;
    interupt_flag*flag_ptr;//这里是指针的原因是 在外面直接访问的是主线程的interupt_flag 不是lambda 所在线程
    public:

    interuptible_thread() = default;
    template <typename F, typename... Args,typename = std::enable_if_t<std::is_invocable_v<F,Args...>>>
    interuptible_thread(F&& f, Args&&... args){
        std::promise<interupt_flag*>p; //使用future 确保构造函数结束完毕后， flag_ptr已经有值
        t = [&](){
            p.set_value(&interupt_flag_this);
            try{
            std::invoke(std::forward<F>(f),std::forward<Args>(args)...);
            }
            catch(std::exception& e){

            }
        };
        flag_ptr = p.get_future().get();
    }
    interuptible_thread(interuptible_thread&&) = default;
    interuptible_thread& operator=(interuptible_thread&&) = default;

    void join(){
        if(t.joinable()){
        t.join();
        }
    }
    void detach(){
        if(t.joinable()){
        t.detach();
        }
    }
    void interrupt(){
        flag_ptr->set();
    }
    ~interuptible_thread(){
        if(t.joinable()){
            t.join();
        }
    }    
};
//内部线程调用
void interupt_point(){ 
    if(interupt_flag_this.is_set()){
        throw std::runtime_error("interupted");
    }
}
//上面版本仍有不足，无法在线程陷入等待的时候被打断，为此进一步封装
//希望能够在 对应flag 被设置的时候也苏醒
// void interuptible_wait_weak(std::condition_variable& cv,std::unique_lock<std::mutex>& lock){
//     struct cleaner{
//         ~cleaner(){
//             interupt_flag_this.del_condition();
//         }
//     };
//     cleaner cl; // 防止空悬指针
    
//     interupt_point();
//     interupt_flag_this.add_condition_unsafe(cv);//
//     cv.wait_for(lock,1ms);
//     interupt_flag_this.del_condition();
//     interupt_point();
// }
// void interuptible_wait(std::condition_variable& cv,std::unique_lock<std::mutex>& lock){
//     while(true){
//         if(interupt_flag_this.is_set()){
//             return;
//         }
//         interuptible_wait_weak(cv,lock);
//     }
// }
//代码的问题：
//如果在88 89之间set 被设置 那么 会导致中断失败。主线程中断。 为此 我们通过锁 不行！！可能死锁！，或者 通过wait_for。
//同时伪唤醒也是有可能的

//解决手段使用 使用std::condtion_variable_any 代替 std::condition_variable
//condition_variable_any 可以绑定任意类型的mutex，我们在这个锁内创建时候lock 住两个：1.外部锁（通过传递方式）2.flag的锁
//然后wait 时候 自然会放开。
template<typename lockable>
struct mutex_for_flag{
    private:
    interupt_flag * _flag;
    lockable & _out_mutex;
    public:
    mutex_for_flag(interupt_flag * flag,lockable & out_mutex,std::condition_variable_any& cv)  :_flag(flag),_out_mutex(out_mutex){
        _flag->add_condition_any(cv);
        _flag->_mutex.lock();
    }
    void lock(){
        std::lock(_out_mutex,_flag->_mutex);//两个一起锁上
    }
    void unlock(){
        _flag->_mutex.unlock();
        _out_mutex.unlock();
    }
    ~mutex_for_flag(){
        _flag->_mutex.unlock();
        _flag->del_condition();
    }
};
void interuptible_wait_weak(std::condition_variable& cv,std::unique_lock<std::mutex>& lock){
    struct cleaner{
        ~cleaner(){
            interupt_flag_this.del_condition();
        }
    };
    cleaner cl; // 防止空悬指针
    
    interupt_point();
    interupt_flag_this.add_condition_unsafe(cv);//
    cv.wait_for(lock,1ms);
    interupt_flag_this.del_condition();
    interupt_point();
}
void interuptible_wait(std::condition_variable& cv,std::unique_lock<std::mutex>& lock){
    while(true){
        if(interupt_flag_this.is_set()){
            return;
        }
        interuptible_wait_weak(cv,lock);
    }
}
template  <typename lockable>
void interuptible_wait(std::condition_variable_any& cv,lockable& lock){
    mutex_for_flag<lockable> flag_mutex(&interupt_flag_this,lock,cv);
    interupt_point();
    cv.wait(flag_mutex);
    interupt_point();
}
//其余类型的等待 可以使用wait_for 轮询的方式


//应用场景 父进程结束 希望子进程也给我结束
//方式 父进程调用interupt 轮询一次 然后join。

