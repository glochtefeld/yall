cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(yall VERSION 1.0)

add_executable(yall src/yall.cpp src/DirectoryInfo.cpp src/argparser.cpp)
target_include_directories(yall PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
