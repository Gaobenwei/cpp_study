cmake_minimum_required(VERSION 3.25)
project(computer_home)

set(CMAKE_CXX_STANDARD 17)

add_executable(computer_home main.cpp identity.h student.cpp student.h teacher.cpp teacher.h manager.cpp manager.h gloablFile.h computerRoom.cpp computerRoom.h orderFile.cpp orderFile.h)
