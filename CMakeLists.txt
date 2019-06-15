cmake_minimum_required(VERSION 3.14)
project(CEDK_Server C)

set(CMAKE_C_STANDARD 99)

add_executable(CEDK_Server main.c server.c server.h)
target_link_libraries(CEDK_Server${pthread} -lpthread)