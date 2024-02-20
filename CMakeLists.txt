cmake_minimum_required(VERSION 3.18.0)

project(flang LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(ENABLE_ASAN ON CACHE BOOL "Enable AddressSanitizer" FORCE)
endif ()

if (ENABLE_ASAN)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
endif ()

add_subdirectory(external)
add_subdirectory(src)
add_subdirectory(tests)