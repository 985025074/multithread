#include "STD.h"
#include "HELPER.h"
#include <netdb.h>
#include <unistd.h>
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT "6666"
#define BUFFER_SIZE 4096
#define CHECK(funcname, ...) check_impl(#funcname, funcname(__VA_ARGS__));
#define DEBUGON
#define DEBUGON_REQUESTHANDLER
#ifdef DEBUGON
#define DEBUG(formats, ...)                                 \
  if (debug)                                                \
  {                                                         \
    std::cout << "DEBUG:" << __LINE__ << "\n";              \
    std::cout << std::format(formats, __VA_ARGS__) << "\n"; \
  }
int debug = 1;
#else
#define DEBUG(formats, ...)
#endif
int check_impl(std::string_view msg, int ret)
{
  if (ret < 0)
  {
    if (msg == "getaddrinfo")
    {
      std::cout << "getaddrinfo:" << gai_strerror(ret) << "\n";
    }
    else
      perror(msg.data());
  }
  fflush(stdout);
  return ret;
}
class addrinfo_container
{
  addrinfo *_ptr;

public:
  addrinfo_container() : _ptr(nullptr)
  {
    CHECK(getaddrinfo, SERVER_IP, SERVER_PORT, NULL, &_ptr);
  }
  addrinfo_container(addrinfo *ptr) : _ptr(ptr)
  {
  }
  [[nodiscard]]
  std::optional<addrinfo *> getAddrinfo()
  {
    addrinfo *ret{};
    if (_ptr)
    {
      ret = _ptr;
      _ptr = _ptr->ai_next;
      return ret;
    }
    return std::nullopt;
  }
  // move constructor
  addrinfo_container(addrinfo_container &&other) : _ptr(std::move(other._ptr))
  {
    other._ptr = nullptr;
  }
  // move assign operator
  addrinfo_container &operator=(addrinfo_container &&other)
  {
    if (_ptr)
      freeaddrinfo(_ptr);
    _ptr = std::move(other._ptr);
    other._ptr = nullptr;
    return *this;
  }
  std::optional<int> createSocketAndBind()
  {
    if (_ptr)
    {
      int soc = socket(_ptr->ai_family, _ptr->ai_socktype, _ptr->ai_protocol);
      if (soc < 0)
      {
        perror("Create Socket Error");
        fflush(stdout);
        return std::nullopt;
      }
      CHECK(bind, soc, _ptr->ai_addr, _ptr->ai_addrlen);
      return soc;
    }
    return std::nullopt;
  }
  ~addrinfo_container()
  {
    if (_ptr)
      freeaddrinfo(_ptr);
  }
};
struct sockaddr_container
{
  // sockaddr addr;
  union
  {
    sockaddr base;
    sockaddr_storage big;
  } addr;
  unsigned int length;
};
std::vector<std::string_view> getLines(std::string_view buffer, std::string_view by = "\r\n")
{
  size_t lastloc = 0;
  size_t loc;
  std::vector<std::string_view> result;

  while ((loc = buffer.find(by, lastloc)) != std::string_view::npos)
  {
    result.emplace_back(buffer.substr(lastloc, loc - lastloc));
    lastloc = loc + by.length();
  }

  result.emplace_back(buffer.substr(lastloc));
  return result;
}
void toLowercase(std::string &src)
{
  for (auto &c : src)
  {
    if (c >= 'A' && c <= 'Z')
    {
      c = c + 32;
    }
  }
}
std::string trim(const std::string& str) {
    size_t start = 0;
    // 找到第一个非空字符的位置
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }

    // 如果字符串全是空格，直接返回空字符串
    if (start == str.size()) {
        return "";
    }

    size_t end = str.size() - 1;
    // 找到最后一个非空字符的位置
    while (end > start && std::isspace(static_cast<unsigned char>(str[end]))) {
        --end;
    }

    // 截取[start, end]区间的子字符串
    return str.substr(start, end - start + 1);
}
class HTTPparser
{
  std::string_view _buffer;
  std::string _method;
  std::string _loc;
  std::string _version;
  std::map<std::string, std::string> _data;
  int _fd = 0;
  bool _get_file = false;
#ifdef DEBUGON_HTTPPARSER
  int debug = 1;
#else
  int debug = 0;
#endif
public:
  HTTPparser() = default;
  HTTPparser(char *_source, int n) : _buffer(_source, n)
  {
    _get_file = true;
  }
  void setFile(char *_source, int n)
  {
    _buffer = std::string_view(_source, n);
    _get_file = true;
  }
  auto &getDataMap()
  {
    return _data;
  }
  bool parse()
  {

    if (!_get_file)
    {
      return false;
    }
    // find head
    size_t locend = _buffer.find("\r\n\r\n");
    if (locend == std::string_view::npos)
    {
      std::cout << "Parse error:head not find!\n"
                << std::endl;
      return false;
    }
    auto lines = getLines(_buffer);
    auto combins = split_by(std::string(lines[0]), ' '); // TODO:better code
    _method = combins[0];
    _loc = combins[1];
    _version = combins[2];
    DEBUG("method:{},loc:{},version:{}", _method, _loc, _version);
    print_container(lines);
    for (size_t i = 1; i < lines.size(); i++)
    {
      auto loc = lines[i].find(":");
      if (loc == std::string_view::npos)
      {
        // std::cout << "Parse error:head syntax error\n"<<std::endl; //open when you need debug+
        continue; // is this right?
      }
      std::string key = std::string(lines[i].substr(0, loc));
      std::string value = std::string(lines[i].substr(loc + 1));
      toLowercase(key);
      toLowercase(value);
      key = trim(key);
      value = trim(value);
      _data[key] = value;
      DEBUG("data:{}:{}", key, value);
    }
    return true;
  }
};
class HTTPresonser
{
#ifdef DEBUGON_HTTPRESONSER
  int debug = 1;
#else
  int debug = 0;
#endif
  std::string _state;
  std::string _version;
  std::string _description;
  std::map<std::string, std::string> _data;
  std::string _message;

public:
  HTTPresonser(std::string_view state, std::string_view version, std::string_view description)
      : _state(state), _version(version), _description(description)
  {
  }
  // return true if insert successed
  bool insertHead(std::string key, std::string value)
  {
    auto result = _data.try_emplace(std::move(key), std::move(value));
    return result.second;
  }
  void appendData(std::string data)
  {
    _message.append(std::move(data));
  }
  std::string getResponse()
  {
    std::string response;
    response.reserve(1024); // TODO:better size;
    response.append(std::format("{} {} {}\r\n", _state, _version, _description));
    for (auto &[k, v] : _data)
    {
      response.append(std::format("{}: {}\r\n", k, v));
    }
    response.append("\r\n\r\n");
    response.append(_message);
    return response;
  }
};
template <typename Parser = HTTPparser>
class HTTPRequestHandler
{
#ifdef DEBUGON_REQUESTHANDLER
  int debug = 1;
#else
  int debug = 0;
#endif
  int _fd;
  std::string _head;
  std::string _data;
  char _buffer[BUFFER_SIZE] = {0};
  int _buffer_length = 0;
  Parser _parser;

public:
  HTTPRequestHandler(int fd) : _fd(fd)
  {
  }
  void readARequset()
  {
    // 初始化
    size_t loc = 0;
    _buffer_length = 0;

    while (true)
    {
      int _read_bytes = CHECK(read, _fd, _buffer, sizeof(_buffer));
      _buffer_length += _read_bytes;
      _head[_buffer_length] = '\0';
      std::string_view _view(_buffer, _buffer_length);
      if ((loc = _view.find("\r\n\r\n", loc)) != std::string_view::npos)
      {
        size_t head_remain = loc - _head.length() + 4;
        _head.append(std::string_view(_buffer + _head.length(), head_remain));
        //-4 是为了移走\r\n\r\n
        _data.append(std::string_view(_buffer + loc +4 , _read_bytes-4 - head_remain));
        break; // head is finished
      }
    }
    _parser.setFile(_head.data(), _head.length());
    _parser.parse();
    size_t content_length;
    try
    {
      std::string _temp = _parser.getDataMap().at("content-length");
      // TODO:check from char return value
      DEBUG("content-length-string:{}", _temp);
      std::from_chars(_temp.data(), _temp.data() + _temp.length(), content_length);
      auto result = std::from_chars(_temp.data(), _temp.data() + _temp.length(), content_length);

      // 检查转换结果
      if (result.ec == std::errc())
      {
        content_length -= _data.length() +4; // 4 是为了移走\r\n\r\n，\r\n是算作正文的？
        DEBUG("content-length: {}", content_length);
      }
      else
      {
        // 如果转换失败，进行错误处理
        DEBUG("Failed to parse content-length from string: {}", _temp);
        // 可以根据需要设置一个默认值或抛出异常等
      }
    }
    catch (...)
    {
      std::cout << "Fail to access content-length!\n";
    }
    while (content_length)
    {
      int _read_bytes = CHECK(read, _fd, _buffer, sizeof(_buffer));

      _head[_buffer_length] = '\0';
      std::string_view _view(_buffer + _buffer_length, _read_bytes);
      _data.append(_view);
      _buffer_length += _read_bytes;
      content_length -= _read_bytes;
    }
    DEBUG("data:{}", _data);
  }
  // move constructor
  HTTPRequestHandler(HTTPRequestHandler &&other) : _fd(std::move(other._fd))
  {
    other._fd = 0;
  }
  // move assign operator
  HTTPRequestHandler &operator=(HTTPRequestHandler &&other)
  {
    if (_fd)
      close(_fd);
    _fd = std::move(other._fd);
    other._fd = 0;
    return *this;
  }

  ~HTTPRequestHandler()
  {
    if (_fd)
      close(_fd);
  }
};
std::vector<join_thread> pool;
int main()
{
  addrinfo_container server;
  int soc = server.createSocketAndBind().value();
  CHECK(listen, soc, SOMAXCONN);
  // 主循环
  std::cout << "Server start\n";
  while (true)
  {
    sockaddr_container client;
    int clientfd = CHECK(accept, soc, &(client.addr.base), &(client.length));
    pool.emplace_back([clientfd]()
                      {
                        // char buffer [1024];
                        // read(clientfd,buffer,1024);
                        // HTTPparser parser(buffer,1024);
                        // parser.parse();
                        HTTPRequestHandler handler(clientfd);
                        handler.readARequset();
                      });
  }

  return 0;
}