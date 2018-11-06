cmake_minimum_required(VERSION 3.10)
project(hw3)

set(CMAKE_CXX_STANDARD 14)

set (HELPER helper.h;helper.cpp)
#add_executable(hw3 main.cpp)
add_executable(build_index build_index.cpp ${HELPER})
add_executable(merge merge.cpp ${HELPER})
#add_executable(test test.cpp ${HELPER})
add_executable(main main.cpp ${HELPER})
