#include "STD.h"
#include "HELPER.h"
#include <netdb.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "80800"
#define PARSER_SIZE 4096
#define CHECK(funcname,...) check_impl(#funcname,funcname(__VA_ARGS__));
#define DEBUG(formats,...) if(debug){\
  std::cout <<"DEBUG:" <<__LINE__<<"\n";\
  std::cout << std::format(#formats,__VA_ARGS__)<<"\n";\
}
int debug = 1;
int check_impl(std::string_view msg,int ret){
  if(ret<0){
    if(msg=="getaddrinfo"){
      std::cout <<"getaddrinfo:"<<gai_strerror(ret) << "\n";
    }
    else 
      perror(msg.data());
  }
  fflush(stdout);
  return ret;
}
class addrinfo_container{
  addrinfo * _ptr;
  public:
  addrinfo_container():_ptr(nullptr){
    CHECK(getaddrinfo,SERVER_IP,SERVER_PORT,NULL,&_ptr);
  }
  addrinfo_container(addrinfo* ptr):_ptr(ptr){

  }
  [[nodiscard]]
  std::optional<addrinfo*> getAddrinfo(){
    addrinfo* ret{};
    if(_ptr){
      ret = _ptr;
      _ptr = _ptr->ai_next;
      return ret;
    }
    return std::nullopt;
  }
  //move constructor
  addrinfo_container(addrinfo_container&& other):_ptr(std::move(other._ptr)){
    other._ptr = nullptr;
  }
  //move assign operator
  addrinfo_container& operator=(addrinfo_container&&other){
    if(_ptr)
      freeaddrinfo(_ptr);
    _ptr = std::move(other._ptr);
    other._ptr=nullptr;
    return *this;
  }
  std::optional<int> createSocketAndBind(){
    if(_ptr){
      int soc = socket(_ptr->ai_family,_ptr->ai_socktype,_ptr->ai_protocol);
      if(soc<0){
        perror("Create Socket Error");
        fflush(stdout);
        return std::nullopt;
      }
      CHECK(bind,soc,_ptr->ai_addr,_ptr->ai_addrlen);
      return soc;
    }
    return std::nullopt;
  }
  ~addrinfo_container(){
    if(_ptr)
      freeaddrinfo(_ptr);
  }
};
struct sockaddr_container{
  // sockaddr addr;
  union{
    sockaddr base;
    sockaddr_storage big;
  }addr;
  unsigned int length;
};
std::vector<std::string_view> getLines(std::string_view buffer, std::string_view by = "\r\n") {
    size_t lastloc = 0;
    size_t loc;
    std::vector<std::string_view> result;

    while ((loc = buffer.find(by, lastloc)) != std::string_view::npos) {
        result.emplace_back(buffer.substr(lastloc, loc - lastloc));
        lastloc = loc + by.length();
    }

    result.emplace_back(buffer.substr(lastloc));
    return result;
}

class HTTPparser{
  std::string_view _buffer;
  std::string _method;
  std::string _loc;
  std::string _version;
  std::map<std::string,std::string>_data;
  int _fd;
  public:
  HTTPparser(char*_source,int n):_buffer(_source,n){
    
  }
  bool parse(){
    //find head
    size_t locend = _buffer.find("\r\n\r\n");
    if(locend == std::string_view::npos){
      std::cout << "Parse error:head not find!\n"<<std::endl;
      return false;
    }

    // size_t first_blank = _buffer.find(" ");
    // size_t second_blank = _buffer.find(" ",first_blank+1);
    // size_t first_line_end = _buffer.find("\r\n",second_blank);
    // DEBUG("firstblank:{},secondblank:{}",first_blank,second_blank);
    // _method =_buffer.substr(0,first_blank-1);
    // _loc = _buffer.substr(first_blank+1,second_blank-first_blank-1);
    // _version = _buffer.substr(second_blank+1,first_line_end-second_blank-1);
    auto lines = getLines(_buffer);
    auto combins = split_by(std::string(lines[0]),' ');//TODO:better code
    _method = combins[0];_loc = combins[1];_version = combins[2];
    DEBUG("method:{},loc:{},version:{}",_method,_loc,_version);
    return true;
  }



};
std::vector<join_thread> pool;
int main(){
  addrinfo_container server;
  int soc = server.createSocketAndBind().value();
  CHECK(listen,soc,SOMAXCONN);
  //主循环
  while(true){
    sockaddr_container client;
    int clientfd = CHECK(accept,soc,&(client.addr.base),&(client.length));
    pool.emplace_back([clientfd](){
      char buffer [1024];
      read(clientfd,buffer,1024);
      HTTPparser parser(buffer,1024);
      parser.parse();
    });
  }

  return 0;
}