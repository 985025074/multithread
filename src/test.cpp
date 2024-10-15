// #include "std.h"
// #include "myPool.h"
// template <typename T,typename = std::void_t<>>
// struct has_size{
//     static constexpr bool value = false;
// };
// template <typename T>
// struct has_size<T,std::void_t<decltype(std::declval<T>().size())>>{
//     static constexpr bool value = true;
// };
// template <typename T,typename =std::enable_if_t<has_size<T>::value>,typename = void>
// void run_(){
//     fmt::println("special");
// }
// template <typename T,typename = std::enable_if_t<!has_size<T>::value>>
// void run_(){
//     fmt::println("default");
// } 
// struct test__{
//     std::vector<int>a;
//     int b;
// };
// void func(test__){
//     fmt::println("test__");
// }
// class hierarchical_lock{
//     public:
//     thread_local static int this_thread_lock_val;
//     int current_lock_val;
//     int previous_lock_val;
//     std::mutex mtx;
//     void check_val(){
//         if(this_thread_lock_val > current_lock_val){
//             throw std::logic_error("try to lock higer lock");
//         }
//     }
//     hierarchical_lock(int val):current_lock_val(val){

//     }
//     void lock(){
//         check_val();
//         mtx.lock();
//         previous_lock_val = this_thread_lock_val;
//         this_thread_lock_val = current_lock_val;
//     }
//     void unlock(){
//         if(current_lock_val != this_thread_lock_val){
//             throw std::logic_error("higer lock isn't unlocked");
//         }
//         this_thread_lock_val = previous_lock_val;
//         mtx.unlock();
//     }
//     bool try_lock(){
//         check_val();
//         bool result = mtx.try_lock();
//         if(result){
//             previous_lock_val = this_thread_lock_val;
//             this_thread_lock_val = current_lock_val;
//             return result;
//         }
//         return false;
//     }
// };
// thread_local int hierarchical_lock::this_thread_lock_val = 0;
// hierarchical_lock highest_lock(1000);
// hierarchical_lock mid_lock(500);
// hierarchical_lock low_lock(1);
// // class thread_queue{
// //     priavte:
// //     std::queue
// // }


// int find_answer(){
//     std::this_thread::sleep_for(std::chrono::seconds(1));
//     return 42;
// }
// struct X{
//     int counter = 0;
//     void inc(){
//         counter++;
//     };
// };
// template <typename T>
// void print(const T& t){
//     std::cout<<"{";
//     std::string blanker="";
//     for(const auto& x:t){
//         std::cout<<std::exchange(blanker,",")<<x;
//     }
//     std::cout<<"}"<<std::endl;

// }
// using std::list;

// // template<typename T>
// // std::list<T> quicksort(std::list<T>&&lis) {
// //     if (lis.size() <= 1) {
// //         return lis;
// //     }
// //     std::list<T> left_part;
// //     T mid_val = lis.front();
// //     lis.pop_front();
// //     auto midright = std::partition(lis.begin(), lis.end(), [mid_val](const T& x) { return x < mid_val; });
// //     left_part.splice(left_part.end(), lis, lis.begin(), midright);
// //     std::future<list<T>> f = std::async(std::launch::async, quicksort<T>, std::move(left_part));

// //     auto right_part = quicksort(std::move(lis));
// //     left_part = f.get();
// //     left_part.insert(left_part.end(), mid_val);
// //     left_part.insert(left_part.end(), right_part.begin(), right_part.end());

// //     return left_part;
// // }
// // #include <random>
// // std::random_device rd;  // 用于生成种子
// // std::mt19937 gen(rd()); // Mersenne Twister 随机数引擎
// // std::uniform_int_distribution<> distrib(1, 10000);
// // bool test_quicksort_impl(){
// //     list<int> lis;
// //     for(int i=0;i<100;i++){
// //         lis.push_back(distrib(gen));
// //     }    
// //     auto sorted_lis = quicksort(list<int>(lis));
// //     std::vector<int> cp_lis (lis.begin(),lis.end());
// //     std::sort(cp_lis.begin(),cp_lis.end());
// //     auto cp_lis_iter = cp_lis.begin();
// //     auto sorted_lis_iter = sorted_lis.begin();
// //     for(;cp_lis_iter!=cp_lis.end();cp_lis_iter++,sorted_lis_iter++){
// //         if(*cp_lis_iter!=*sorted_lis_iter){
// //             return false;
// //         }
// //     }
// //     return true;
// // }
// // void test_quicksort(){
// //     bool result = true;
// //     for(int i=0;i<100;i++){
// //         result &= test_quicksort_impl();
// //     }
// //     fmt::print("test_quicksort result:{}\n",result);
// // }

// // template <typename Func,typename ...Args,typename = decltype(std::declval<Func>()(std::forward<Args>(std::declval<Args>())...))>
// // std::true_type callable (void*);
// // template <typename Func,typename ...Args>
// // std::false_type callable (...);
// // template <typename Func>
// // auto maker(Func && ruler){
// //     return [](auto && t){
// //         return decltype(callable<Func,decltype(get_T(t))>(nullptr))::value;
// //     };
// // }
// // template <typename T>
// // struct Type{
// //     using type = T;
// // };
// // template <typename T>
// // auto type = Type<T>();
// // template <typename T>
// // T get_T(Type<T>);


// // struct Base{
// //     Base(){
// //         Init();
// //     }
// //     void Init(){
// //         f();
// //     }   
// //     virtual void f(){
// //         fmt::print("Base::f()\n");
// //     }
// // };
// // struct Son:public Base{
// //     void f(){
// //         fmt::print("Son::f()\n");
// //     }
// // };

// class ss final{
//     public:
//     ss() =default;
//     ss(const ss&){
//         fmt::println("copy constructor");
//     }
//     ss& operator=(const ss&){
//         fmt::println("copy constructor by =");
//     }
//     ss(ss&&)
//     {
//         fmt::println("move constructor");
//     }
//     ss& operator=(ss&&)
//     {
//         fmt::println("move constructor by =");
//     }
// };
// class checker{
//     public:
//     checker(int) {
//         fmt::println("int constructor");
//     }
//     checker() = default;
//     checker(const checker&){
//         fmt::println("copy constructor");
//     }
//     checker& operator=(const checker&){
//         fmt::println("copy constructor by =");
//         return *this;
//     }
//     checker(checker&&)
//     {
//         fmt::println("move constructor");
//     }
//     checker& operator=(checker&&)
//     {
//         fmt::println("move constructor by =");
//         return *this;
//     }
//     ~checker(){
//         puts(__PRETTY_FUNCTION__);
//     }
// };
// class test_func{
//     public:
//     checker c;
//     test_func() = default;
//     test_func(test_func&& other):c(std::move(other.c)){

//     }
//     // ~test_func(){

//     // }
// };
// template <typename T>
// auto is_int(T a){
//     return std::false_type();
// };
// auto is_int(int a){
//     return std::true_type();
// }
// template <typename T>
// void func_impl(T&&,std::true_type){
//     puts(__PRETTY_FUNCTION__);
// }
// void func_impl(int,std::false_type){
//     puts(__PRETTY_FUNCTION__);
// }
// template <typename T>
// void func(T&&a){
//     func_impl(std::forward<T>(a),is_int(a));
// }
// std::vector<int> v =  {1,2,3,4,5};
// template <char ...>
// struct _s_impl;
// template <char c,char ...T>
// struct _s_impl<c,T...>{
//     static  constexpr  auto value = std::array<char,2+sizeof...(T)>{c,T...};
// };

// template <char ... T>
// constexpr auto operator "" _s(){
//     return std::string((const char*)_s_impl<T...>::value.data());

// }
// using namespace std::literals;
// std::promise<void>p;

// void task(int i,std::shared_future<void> f){
//     f.wait();
//     std::cout << "task"<< i << std::endl;
// }
// template <unsigned i,int answer>
// struct print_answer{
//     void operator()(){

//     }
// };

// #include "safe_queue.h"
// threadsafe_queue<int> q;
// normal_queue<int> q2;
// books::threadsafe_queue<int> q3;
// template <typename container>
// void create(int num,container& q){
//     for(int i =0;i<num;i++){
//         q.push(i);
//     }
// }
// template <typename container>
// void del(int num,container& q){
//     for(int i =0;i<num;i++){
//         q.wait_and_pop();
//     }
// }
// template <typename container>
// void benchmark(int times,container& p){
//     int64_t result =0;
    
//     auto now = syc::Timer();
//     for(int i =0;i<times;i++){
//     std::vector<std::thread>pool;
//     pool.emplace_back(create<container>,20000,std::ref(p));
//     pool.emplace_back(create<container>,10000,std::ref(p));
//     pool.emplace_back(create<container>,10000,std::ref(p));
//     pool.emplace_back(del<container>,40000,std::ref(p));
    
//     for(auto& t:pool){
//         t.join();
//     }
//         result+= now.stop();
//     }
//     result/=times;
//     std::cout << "benchmark result:" << result << std::endl;
// }
// #include "std.h"
// #include "helper.h"
// template <typename T>
// std::list<T> quicksort(std::list<T> lis) {
//     if(lis.empty()) return lis;
//     std::list<T>final_list;
//     final_list.splice(final_list.end(),lis,lis.begin()); // move the first element to!
//     auto loc = std::partition(lis.begin(),lis.end(),[&final_list](const T& val){
//         return val < *final_list.begin();
//     });
//     std::list<T> left_part;
//     list_part.splice(left_part.end(),lis,lis.begin(),loc);
//     print_container(left_part);
//     print_container(lis);
// }
// void speaker(int i){
//     std::cout << "speaker " << i << std::endl;
// }
int main(){
    // std::list<int> lis = {5,3,8,1,9,2,7,4,6};
    // quicksort(lis);
    
    return 0;
}
