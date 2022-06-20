cmake_minimum_required(VERSION 3.14)

project(MaximumClique VERSION 1.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(CMAKE_SYSTEM MATCHES Windows)
    # string(REPLACE "/RTC1" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    add_compile_options(/O2 /openmp)
endif()
if(CMAKE_SYSTEM MATCHES Linux)
    add_compile_options(-march=native -O3)
endif()
set(Boost_USE_STATIC_LIBS ON)

file(GLOB src_graph "*.c" "*.cpp")

find_package(Boost 1.77.0 REQUIRED COMPONENTS json graph)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost library wasn't found.")
endif()

include_directories(${Boost_INCLUDE_DIRS})

add_executable(MaximumClique ${src_graph})

target_link_libraries(MaximumClique LINK_PUBLIC
                      ${Boost_LIBRARIES})
target_include_directories(MaximumClique PUBLIC
                           "${PROJECT_BINARY_DIR}")
