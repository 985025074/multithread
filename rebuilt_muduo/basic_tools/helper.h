#pragma once
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>
#include <utility>
namespace learn{
    class unmovable{
    public:
    unmovable() = default;
    unmovable( unmovable&&) = delete;
    unmovable& operator=(unmovable&&) = delete;
    unmovable(const unmovable&) = default;
    unmovable& operator=(const unmovable&) = default;
};
class uncopyable{
    public:
    uncopyable() = default;
    uncopyable( uncopyable&&) =default;
    uncopyable& operator=(uncopyable&&) =default;
    uncopyable(const uncopyable&) = delete;
    uncopyable& operator=(const uncopyable&) = delete;
};
};
template <typename T>
struct __{
    T value;
};
template <typename T>
std::ostream& operator<<(std::ostream& os, const __<T>& x){
    os << x.value << " ";
    return os;
}
template <typename...T>
void print(T&&... args){
    (std::cout << ... << __(args)) << std::endl;
}
#define WATCH(...) print(__FILE__,__LINE__,"WATCH_POINT: ",__VA_ARGS__);
#define LINE std::cout <<"--------------------------"<<std::endl;
template <typename Container>
void print_container(Container const& c) {
    std::string func_name (__PRETTY_FUNCTION__);
   
    std::cout << "Container:"<<func_name.substr(57,func_name.rfind("]")-57) << std::endl;
    std::cout << "[" ;
    char delimeter = ' ';
    for (auto const& e : c) {
        std::cout <<delimeter<<e;
        std::exchange(delimeter, ',');
    }
    std::cout << " ]" << "\n";
}

class join_thread{
public:
    join_thread() = default;
    template <typename F, typename... Args,typename = std::enable_if_t<std::is_invocable_v<F,Args...>>>
    join_thread(F&& f, Args&&... args):t(std::forward<F>(f), std::forward<Args>(args)...){

    }
    join_thread(join_thread&&) = default;
    join_thread& operator=(join_thread&&) = default;

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
    bool joinable(){
        return t.joinable();
    }
    ~join_thread(){
        if(t.joinable()){
            t.join();
        }
    }
private:
    std::thread t;
};
//study
class notice{
    public:
    notice() =default;
    notice(const notice&){
        puts(__PRETTY_FUNCTION__);
    }
    notice& operator=(const notice&){
        puts(__PRETTY_FUNCTION__);
        return *this;
    }
    notice(notice&&)
    {
        puts(__PRETTY_FUNCTION__);
    }
    notice& operator=(notice&&)
    {
        puts(__PRETTY_FUNCTION__);
        return *this;
    }
    ~notice(){
        puts(__PRETTY_FUNCTION__);
    }
};
// inline std::vector<std::string> read_file(const char* filename){
//     std::ifstream ifs(filename);
//     if(!ifs.is_open()){
//         fmt::println("open file [{}] failed,error:{}",filename,strerror(errno));
//         return {};
//     }
//     std::vector<std::string> lines;
//     std::string line;
//     while(getline(ifs,line)){
//         lines.push_back(std::move(line));
//     }
//     return lines;
// }
inline std::vector<std::string> split_by(std::string s,char decl){
    std::istringstream oss(s);
    std::string item;
    std::vector<std::string> res;
    while(getline(oss,item,decl)){
        res.push_back(std::move(item));
    }
    return res;
}