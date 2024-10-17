#pragma once
#include "std.h"
#include "lock_free_stack_shared.h"
namespace hazard_pointer{
//hazard方案保证及时释放
    const int max_threads_together = 10;
struct hazard_pointer{//指涉当前线程访问对象
    std::atomic<void*>ptr;
    std::atomic<std::thread::id> owner;
};
hazard_pointer hptrs[max_threads_together];
class hpkeeper{
    hazard_pointer* hptr;
    public:
    hpkeeper(){
        for(int i=0;i<max_threads_together;i++){
            auto default_id = std::thread::id();
            if(hptrs[i].owner.compare_exchange_strong(default_id,std::this_thread::get_id())){
                hptr = &hptrs[i];
                return;
            }
        }
        throw std::runtime_error("no enough hazard pointer for this thread");
    }
    auto get_hptr(){
        return hptr;
    }
    ~hpkeeper(){
        
        hptr->ptr.store(nullptr);
        hptr->owner.store(std::thread::id());
       
    }
};

hpkeeper& get_this_thread_hp(){
    static thread_local hpkeeper ptr;
    return ptr;
}
bool safe_one(void*ptr){//直接传入具体的数据指针更块
    for(int i=0;i<max_threads_together;i++){
        if(hptrs[i].ptr.load()==ptr){
            return false;
        }
    }
    return true;
}
template <typename T>
class lock_free_stack {
    struct node{ 
        std::shared_ptr<T> data;
        node* next;
        node(T data,node* next=nullptr):data(std::make_shared<T>(data)),next(std::move(next)){};
    };
    struct delete_node{
        node*data;   
        delete_node* next;
        delete_node(node* data,delete_node* next=nullptr):data(data),next(next){};
        ~delete_node(){
            delete data;
        }
    };
    std::atomic<delete_node*> wait_to_die;
    void add_to_wait_to_die(node* data){
        delete_node* newnode = new delete_node(data,wait_to_die.load());
        while(!wait_to_die.compare_exchange_weak(newnode->next,newnode));
    }
void delete_safe_nodes(){
    
    delete_node* temp = wait_to_die.exchange(nullptr);
    while(temp){
        auto next = temp->next;
        if(safe_one(temp->data)){
            delete temp;
        }
        else{
            add_to_wait_to_die(temp->data);
        }
        temp =next;
    }
    
}
    std::atomic<node*> head;
    public:
        
        lock_free_stack():head(nullptr){

        }
        //定义移动自动删除拷贝
        lock_free_stack(lock_free_stack&& other) = default;
        lock_free_stack& operator=(lock_free_stack&& other) = default;
        void push(T data){//push 直接进去就完了
            node*newnode = new node(std::move(data),head.load());
            while(!head.compare_exchange_weak(newnode->next,newnode));
        }
        std::shared_ptr<T> pop(){
            
            hpkeeper& hp =get_this_thread_hp();
            node * oldhead = head.load();
            node * temp;
            
            do{
                do{temp =oldhead;
                hp.get_hptr()->ptr.store(oldhead);//风险指针置为
                oldhead = head.load();
                }while(temp!=oldhead); //读取指针 再设置风险指针，中间存在一定间隙 非常有可能 中间这一点时间这个点就被删了
            }while(oldhead&&!head.compare_exchange_strong(oldhead,oldhead->next));
            
            hp.get_hptr()->ptr.store(nullptr);
            std::shared_ptr<T> data;
            if(oldhead){
                data=std::move(oldhead->data);
             add_to_wait_to_die(oldhead);

            }
            delete_safe_nodes();
            
            return data;
        }
        
};
};
template <typename T>
class lock_free_stack {
    struct node{ 
        std::shared_ptr<T> data;
        node* next;
        node(T data,node* next=nullptr):data(std::make_shared<T>(data)),next(std::move(next)){};
    };
    std::atomic<node*> head;
    std::atomic<int>num_pop;
    std::atomic<node*> wait_to_die;
    
    void kill_all_nodes(node* ptr){
      while(ptr){
        killed++;
        node* temp = ptr->next;
        delete ptr;
        ptr = temp;
      }
    }
    void try_delete(node* oldhead){
      //num pop 不能放在这里 不能保证 这个线程一定是最后一个
      if(num_pop== 1){ 
            node* first =wait_to_die.exchange(nullptr);//我不希望我在删除的时候再加入别的节点
            //这里要在检查一次 获取完 wait_to_die 有没有新人入伙呢？
            if(!--num_pop){//如果此处判断失败 说明拿完上面这个权限 又有新的pop 来了。
                kill_all_nodes(first);
            }
            else if(first){
                // //交还 XXX
                // wait_to_die.store(first);
                //不能直接这样交换 可能wait_to_die里已经存了一些
                pendding_nodes(first);//添加到前面.
            }
            delete oldhead;
            killed++;
        }
        else{
           
            pendding_node(oldhead); //顺序别换，不然你先--了 别人直接开删了
            num_pop--;
            
        }
    }
    public:
        std::atomic<int>killed;
        lock_free_stack():head(T()){

        }
        //定义移动自动删除拷贝
        lock_free_stack(lock_free_stack&& other) = default;
        lock_free_stack& operator=(lock_free_stack&& other) = default;
        void push(T data){
            node*newnode = new node(std::move(data),head.load());
            while(!head.compare_exchange_weak(newnode->next,newnode));
        }
        std::shared_ptr<T> pop(){
            num_pop++;
            node * oldhead = head.load();
            while(oldhead&&!head.compare_exchange_weak(oldhead,oldhead->next));
            //这里就要看看删不删了
            std::shared_ptr<T> data;
            if(oldhead){
              data =std::move(oldhead->data);
             //不能在这里加 没有顺序保证
            }//只有非空节点才会更新数据 但是如果最后结果是空栈也要尝试去delete 因为你可能是最后一个进入pop的
            try_delete(oldhead);
            return data;
        }
        void pendding_nodes(node* nodes){//按原序加入列表
            if(!nodes) return;
            node* temp =nodes;       
            while(temp->next){
                temp = temp->next;
            };
            pendding_nodes(nodes,temp);
        }
        void pendding_nodes(node*start,node*last){
            last->next = wait_to_die.load();
            while((!wait_to_die.compare_exchange_weak(last->next,start)));
        }
        void pendding_node(node* node){
            pendding_nodes(node,node);
        }
        
};
namespace fortest_stack{
    void test(){
        int m[150000] = {0};
        lock_free_stack_shared<int> s;
        std::thread t1([&](){
            for(int i=0;i<150000;i++){
                s.push(i);
            }
        });
        t1.join();
        std::thread t2([&](){
            for(int i=0;i<50000;i++){
                
            auto data = s.pop();
            m[*data] +=1;
            std::cout << *data <<std::endl;
            
            }
        });
        std::thread t3([&](){
            for(int i=0;i<50000;i++){
            auto data = s.pop();
            m[*data] +=1;
            std::cout << *data <<std::endl;
            }
        });
        std::thread t4([&](){
            for(int i=0;i<50000;i++){
            auto data = s.pop();
            m[*data] +=1;
            std::cout << *data <<std::endl;
            }
        });
        t2.join();
        t3.join();
        t4.join();
        for(int i=0;i<150000;i++){
            if(m[i]!=1){
                std::cout << m[i] <<std::endl;
                std::cout <<i<< " error" <<std::endl;
            }
        }
        std::cout << "finish" <<std::endl;
    }
}