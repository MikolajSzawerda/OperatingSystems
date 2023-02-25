cmake_minimum_required(VERSION 3.24)
project(soi_lab4)

set(CMAKE_CXX_STANDARD 17)

add_executable(soi_lab4 main.cpp )
target_link_libraries(soi_lab4 pthread)
