cmake_minimum_required(VERSION 3.10)
project(BranchBench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(ROOT REQUIRED COMPONENTS RIO Tree)

include(${ROOT_USE_FILE})

add_executable(io_benchmark src/main.cpp)

target_link_libraries(io_benchmark ${ROOT_LIBRARIES})

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(io_benchmark OpenMP::OpenMP_CXX)
endif()
