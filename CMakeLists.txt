cmake_minimum_required(VERSION 4.0)

project(OSLab4)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)


add_executable(reciver reciver.cpp)
add_executable(sender sender.cpp)

target_compile_features(reciver PRIVATE cxx_std_20)
target_compile_features(sender PRIVATE cxx_std_20)