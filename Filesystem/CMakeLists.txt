cmake_minimum_required(VERSION 3.24)
project(lab6 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab6 main.c filesystem.h init.h block.h type.h file.h copy.h remove.h represent.h save.h)

target_link_libraries(lab6 m)
