set(base_SRCS
    LogStream.cpp
    FormatTools.cpp
    Logger.cpp
    )

add_library(basic_tools ${base_SRCS})
target_link_libraries(basic_tools pthread rt)


install(TARGETS basic_tools DESTINATION ${PROJECT_SOURCE_DIR}/lib)


file(GLOB HEADERS "*.h")

install(FILES ${HEADERS} DESTINATION ${PROJECT_SOURCE_DIR}/muduo_include)

if(MUDUO_BUILD_EXAMPLES)
  add_subdirectory(tests)
endif()
