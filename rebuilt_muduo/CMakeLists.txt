#project会设置编译器！ 所以标志也会被设置，set一定要在project 之前
CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(rebuilt_muduo)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# includes
# include_directories(${PROJECT_SOURCE_DIR})
#add other directories here
add_subdirectory(basic_tools)

add_library(muduo_lib STATIC empty.cpp)

target_link_libraries(muduo_lib PUBLIC basic_tools)
target_include_directories(muduo_lib PUBLIC ${PROJECT_SOURCE_DIR})




