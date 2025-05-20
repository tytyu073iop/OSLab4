cmake_minimum_required(VERSION 4.0)

project(OSLab4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(reciver reciver.cpp)
add_executable(sender sender.cpp)