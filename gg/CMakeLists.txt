cmake_minimum_required(VERSION 2.8.12)

set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

project(GraphGen CXX)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(gg
  ${SRC_DIR}/main.cpp
  )

set_property(TARGET gg PROPERTY CXX_STANDARD 11)
set_property(TARGET gg PROPERTY CXX_STANDARD_REQUIRED ON)