cmake_minimum_required(VERSION 3.18.0)

project(flang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)