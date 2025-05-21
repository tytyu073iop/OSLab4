cmake_minimum_required(VERSION 4.0)

project(OSLab4)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(reciver reciver.cpp)
add_executable(sender sender.cpp)
add_library(addons addons.cpp)
target_include_directories(addons PUBLIC ${CMAKE_SOURCE_DIR})
target_link_libraries(reciver addons)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#test
enable_testing()

add_executable(addonsTest addonstest.cpp)
target_link_libraries(addonsTest GTest::gtest_main addons)

include(GoogleTest)
gtest_discover_tests(addonsTest)