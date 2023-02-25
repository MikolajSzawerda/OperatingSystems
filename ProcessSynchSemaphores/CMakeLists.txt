cmake_minimum_required(VERSION 3.16.3)
project(lab3 C)

set(CMAKE_C_STANDARD 11)


add_executable(main main.c shared.h)
add_executable(producer producer.c shared.h)
add_executable(consumer consumer.c shared.h)

target_link_libraries(producer pthread)
target_link_libraries(main pthread)
target_link_libraries(consumer pthread)