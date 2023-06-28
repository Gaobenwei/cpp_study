

cmake_minimum_required(VERSION 3.25.0)
project(bank)

set(CMAKE_CXX_STANDARD 17)

add_executable(bank main.cpp user.cpp user.h admin.h admin.cpp)
