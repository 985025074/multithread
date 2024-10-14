#pragma once
#include "std.h"
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
        }
        else{
            pendding_node(oldhead); //顺序别换，不然你先--了 别人直接开删了
            num_pop--;
        }
    }
    public:
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
        int m[1000000] = {0};
        lock_free_stack<int> s;
        std::thread t1([&](){
            for(int i=0;i<1000000;i++){
                s.push(i);
            }
        });
        t1.join();
        std::thread t2([&](){
            for(int i=0;i<500000;i++){
                
               m[*s.pop()] +=1;
            
            }
        });
        std::thread t3([&](){
            for(int i=0;i<500000;i++){
              m[*s.pop()] +=1;
            }
        });
        t2.join();
        t3.join();
        for(int i=0;i<1000000;i++){
            if(m[i]!=1){
                std::cout << m[i] <<std::endl;
                std::cout <<i<< " error" <<std::endl;
            }
        }
        std::cout << "finish" <<std::endl;
    }
}