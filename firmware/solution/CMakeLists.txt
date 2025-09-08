cmake_minimum_required(VERSION 3.22)
project(solution)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(answer main.cpp)

# Include dbcppp headers and link library directly
target_include_directories(answer PRIVATE /usr/local/include/dbcppp)
target_link_libraries(answer PRIVATE /usr/local/lib/libdbcppp.so)


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()
