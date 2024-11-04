// //version 1.0
// #pragma once
// #include <iostream>
// #include <ranges>
// #include <utility>
// #include <version>
// #include <vector>
// #include <thread>
// #include <stack>
// #include <mutex>
// #include <queue>
// #include <chrono>
// #include <future>
// #include <memory>
// #include<list>
// #include <functional>
// #include <cassert>
// #include <algorithm>
// #include <thread>
// #include <vector>
// #include <map>
// #include <memory>
// #include <chrono>
// #include <iostream>
// #include <future>
// #include <shared_mutex>
// #include <condition_variable>
// #include <mutex>
// #include <numeric>
// #include <fstream>
// #include <random>
// namespace syc{
// struct Timer{
//     std::chrono::steady_clock::time_point last_point;
//     Timer(){
//         last_point = std::chrono::steady_clock::now();
//     }
//     auto stop(){
//         auto now= std::chrono::steady_clock::now();
//         auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_point).count();
//         last_point = now;
//         return duration;
//     }
// };
// }

// namespace learn{
//     class unmovable{
//     public:
//     unmovable() = default;
//     unmovable( unmovable&&) = delete;
//     unmovable& operator=(unmovable&&) = delete;
//     unmovable(const unmovable&) = default;
//     unmovable& operator=(const unmovable&) = default;
// };
// class uncopyable{
//     public:
//     uncopyable() = default;
//     uncopyable( uncopyable&&) =default;
//     uncopyable& operator=(uncopyable&&) =default;
//     uncopyable(const uncopyable&) = delete;
//     uncopyable& operator=(const uncopyable&) = delete;
// };
// };
// template <typename T>
// struct __{
//     T value;
// };
// template <typename T>
// std::ostream& operator<<(std::ostream& os, const __<T>& x){
//     os << x.value << " ";
//     return os;
// }
// template <typename...T>
// void print(T&&... args){
//     (std::cout << ... << __(args)) << std::endl;
// }
// #define WATCH(...) print(__FILE__,__LINE__,"WATCH_POINT: ",__VA_ARGS__);
// #define LINE std::cout <<"--------------------------"<<std::endl;
// template <typename Container>
// void print_container(Container const& c) {
//     std::string func_name (__PRETTY_FUNCTION__);
   
//     std::cout << "Container:"<<func_name.substr(57,func_name.rfind("]")-57) << std::endl;
//     std::cout << "[" ;
//     char delimeter = ' ';
//     for (auto const& e : c) {
//         std::cout <<delimeter<<e;
//         std::exchange(delimeter, ',');
//     }
//     std::cout << " ]" << "\n";
// }

// class join_thread{
// public:
//     join_thread() = default;
//     template <typename F, typename... Args,typename = std::enable_if_t<std::is_invocable_v<F,Args...>>>
//     join_thread(F&& f, Args&&... args):t(std::forward<F>(f), std::forward<Args>(args)...){

//     }
//     join_thread(join_thread&&) = default;
//     join_thread& operator=(join_thread&&) = default;

//     void join(){
//         if(t.joinable()){
//         t.join();
//         }
//     }
//     void detach(){
//         if(t.joinable()){
//         t.detach();
//         }
//     }
//     bool joinable(){
//         return t.joinable();
//     }
//     ~join_thread(){
//         if(t.joinable()){
//             t.join();
//         }
//     }
// private:
//     std::thread t;
// };
// //study
// class notice{
//     public:
//     notice() =default;
//     notice(const notice&){
//         puts(__PRETTY_FUNCTION__);
//     }
//     notice& operator=(const notice&){
//         puts(__PRETTY_FUNCTION__);
//         return *this;
//     }
//     notice(notice&&)
//     {
//         puts(__PRETTY_FUNCTION__);
//     }
//     notice& operator=(notice&&)
//     {
//         puts(__PRETTY_FUNCTION__);
//         return *this;
//     }
//     ~notice(){
//         puts(__PRETTY_FUNCTION__);
//     }
// };
// inline std::vector<std::string> read_file(const char* filename){
//     std::ifstream ifs(filename);
//     if(!ifs.is_open()){
//        std::cout << std::format("open file [{}] failed,error:{}\n",filename,strerror(errno));;
//         return {};
//     }
//     std::vector<std::string> lines;
//     std::string line;
//     while(getline(ifs,line)){
//         lines.push_back(std::move(line));
//     }
//     return lines;
// }
// inline std::vector<std::string> split_by(std::string s,char decl){
//     std::istringstream oss(s);
//     std::string item;
//     std::vector<std::string> res;
//     while(getline(oss,item,decl)){
//         res.push_back(std::move(item));
//     }
//     return res;
// }
// template <typename ...T>
// class overload:public T...{
//     public:
//     using T::operator()...;
    
// };
// class RandomGenerator{
//     public:
//     using IntGenerator = std::uniform_int_distribution<int>;
//     using FloatGenerator = std::uniform_real_distribution<float>;
//     RandomGenerator():gen(std::random_device{}()),dis(std::monostate()){
        
//     }
//     void use_int(){
//         dis = IntGenerator();
//     }
//     void use_float(){
//         dis = FloatGenerator();
//     }
//     void set_range(int min,int max){
//         std::visit(overload{
//             [this,min,max](IntGenerator& dis){this->dis = IntGenerator(min,max);},
//             [this,min,max](FloatGenerator& dis){this->dis = FloatGenerator(min,max);},
//             [this](auto&){std::cout <<"No Generator!Set first.\n";},
//             },dis);
//     }
//     int uniform_int(){
//         try{
//             return std::get<IntGenerator>(dis)(gen);
//         }
//         catch(...){
//             std::cout <<"Not Int Generator\n";
//             return 0;
//         }
//     }
//     float uniform_float(){
//         try{
//             return std::get<FloatGenerator>(dis)(gen);
//         }
//         catch(...){
//             std::cout <<"Not Float Generator\n";
//             return 0;
//         }     
//     }
//     private:
//     std::mt19937 gen;
//     std::variant<std::monostate,IntGenerator,FloatGenerator> dis;
// };
// class EasyTimer{
//     void get_time(){
        
//     }


// };
// class GeneralDuration
// {
// public:
//     // TODO 枚举反射
//     enum nowType
//     {
//         HOURS,
//         MINUTES,
//         SECONDS,
//         MILLISECONDS
//     };
//     friend std::ostream &operator<<(std::ostream &os, const GeneralDuration &d);
//     using milliseconds = std::chrono::milliseconds;
//     using seconds = std::chrono::seconds;
//     using minutes = std::chrono::minutes;
//     using hours = std::chrono::hours;
//     GeneralDuration() = default;
//     GeneralDuration(milliseconds ms) : _time(ms), _type(nowType::MILLISECONDS)
//     {
//     }
//     GeneralDuration(seconds s) : _time(s), _type(nowType::SECONDS)
//     {
//     }
//     GeneralDuration(minutes m) : _time(m), _type(nowType::MINUTES)
//     {
//     }
//     GeneralDuration(hours h) : _time(h), _type(nowType::HOURS)
//     {
//     }
//     template <typename T>
//     T turnTo() const
//     {
//         return std::visit(
//             overload{
//                 [](hours _hour)
//                 { return std::chrono::duration_cast<T>(_hour); },
//                 [](minutes _minutes)
//                 { return std::chrono::duration_cast<T>(_minutes); },
//                 [](seconds _seconds)
//                 { return std::chrono::duration_cast<T>(_seconds); },
//                 [](milliseconds _hour)
//                 { return std::chrono::duration_cast<T>(_hour); },
//             },
//             _time);
//     }
//     bool compare_less(const GeneralDuration &other) const
//     {
//         return std::visit(
//             overload{
//                 [&](auto _time, auto _time2)
//                 {
//                     // 转换到最小单位（milliseconds）后比较
//                     return std::chrono::duration_cast<milliseconds>(_time) <
//                            std::chrono::duration_cast<milliseconds>(_time2);
//                 }},
//             _time, other._time);
//     }

//     GeneralDuration operator-(const GeneralDuration &other) const
//     {
//         return std::visit(
//             overload{
//                 [&](auto _time, auto _time2)
//                 {
//                     // 转换到最小单位（milliseconds）后计算差值
//                     auto diff = std::chrono::duration_cast<milliseconds>(_time) -
//                                 std::chrono::duration_cast<milliseconds>(_time2);
//                     return GeneralDuration(diff);
//                 }},
//             _time, other._time);
//     }
//     GeneralDuration operator +(const GeneralDuration &other) const{
//                 return std::visit(
//             overload{
//                 [&](auto _time, auto _time2)
//                 {
//                     // 转换到最小单位（milliseconds）后计算差值
//                     auto diff = std::chrono::duration_cast<milliseconds>(_time) +
//                                 std::chrono::duration_cast<milliseconds>(_time2);
//                     return GeneralDuration(diff);
//                 }},
//             _time, other._time);
//     }
//     bool operator<(const GeneralDuration &other) const
//     {
//         return compare_less(other);
//     };
//     bool operator>(const GeneralDuration &other) const
//     {
//         return other.compare_less(*this);
//     }
//     bool operator>=(const GeneralDuration &other) const
//     {
//         return !(*this < other);
//     }

//     bool operator<=(const GeneralDuration &other) const
//     {
//         return !(*this > other);
//     }
//     operator hours() const
//     {
//         return turnTo<hours>();
//     }
//     operator minutes() const
//     {
//         return turnTo<minutes>();
//     }
//     operator seconds() const
//     {
//         return turnTo<seconds>();
//     }
//     operator milliseconds() const
//     {
//         return turnTo<milliseconds>();
//     }
//     nowType getType() const
//     {
//         return _type;
//     }

// private:
//     std::variant<milliseconds, seconds, minutes, hours> _time;
//     nowType _type;
// };
// // 下面包含在文件中
// inline std::ostream &operator<<(std::ostream &os, const GeneralDuration &d)
// {
//     switch (d.getType())
//     {
//     case GeneralDuration::nowType::HOURS:
//         os << std::chrono::hours(d).count() << " hours";
//         break;
//     case GeneralDuration::nowType::MINUTES:
//         os << std::chrono::minutes(d).count() << " min";
//         break;
//     case GeneralDuration::nowType::SECONDS:
//         os << std::chrono::seconds(d).count() << " seconds";
//         break;
//     case GeneralDuration::nowType::MILLISECONDS:
//         os << std::chrono::milliseconds(d).count() << " milliseconds";
//         break;
//     default:
//         os << "unknown type";
//         break;
//     }
//     return os;
// }