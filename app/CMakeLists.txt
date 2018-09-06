cmake_minimum_required(VERSION 3.12)
project(app C)

set(CMAKE_C_STANDARD 99)

add_executable(app requestor.c)
target_link_libraries(app curl)