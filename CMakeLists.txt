# project会设置编译器！ 所以标志也会被设置，set一定要在project 之前
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_COMPILER "gcc")

CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(fortest)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# includes
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/sycserver)
# refresh src
aux_source_directory(${PROJECT_SOURCE_DIR}/src TEMP)
list(APPEND SRC_CPP ${TEMP})

list(APPEND SRC_CPP ${TEMP})
#set target
add_executable(hello ${SRC_CPP})
add_executable(unit_test ${PROJECT_SOURCE_DIR}/unit_test/unit_test.cpp)
add_executable(bm ${PROJECT_SOURCE_DIR}/benchmark/benchmark.cpp)
# using C++ 
target_compile_features(hello PUBLIC cxx_std_20)
target_compile_options(hello PUBLIC -fmodules-ts)
message(SRC_CPP)
target_compile_options(hello PUBLIC -Wall -Wextra -Wpedantic -Werror -g)
# target_compile_definitions(hello PUBLIC HEAD=1)
# target_sources(hello PUBLIC  FILE_SET CXX_MODULES FILES include/mod.ixx )
# use lib
add_subdirectory(otherlib/fmt)
target_link_libraries(hello fmt::fmt)
target_link_libraries(unit_test fmt::fmt)
target_link_libraries(bm fmt::fmt)
# 注意两个都要
# add_subdirectory(otherlib/googletest)
# target_link_libraries(unit_test gtest_main)
# target_link_libraries(unit_test gtest)
find_package(GTest)
target_link_libraries(unit_test GTest::gtest_main GTest::gtest)
find_package(benchmark)
target_link_libraries(bm benchmark::benchmark)
#设置为release模式

# link libs and dirs
add_subdirectory(rebuilt_muduo)
target_link_libraries(hello muduo_lib)




# real muduo

target_link_libraries(hello /usr/local/lib/libmuduo_base.a)


