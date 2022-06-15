cmake_minimum_required(VERSION 3.14)

project(Graph_coloring VERSION 1.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-march=native -O3)
set(Boost_USE_STATIC_LIBS ON)

file(GLOB src_graph "*.c" "*.cpp")

find_package(Boost 1.77.0 REQUIRED COMPONENTS json graph)
if (NOT Boost_FOUND)
    message(FATAL_ERROR "Boost library wasn't found.")
endif()

include_directories(${Boost_INCLUDE_DIRS})

add_executable(Graph_coloring ${src_graph})

target_link_libraries(Graph_coloring LINK_PUBLIC
                      ${Boost_LIBRARIES})
target_include_directories(Graph_coloring PUBLIC
                           "${PROJECT_BINARY_DIR}")
