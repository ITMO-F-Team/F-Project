add_library(flang
    flang/token.cpp
)

target_include_directories(
    flang
    SYSTEM
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_compile_options(flang
    PRIVATE
        $<$<CONFIG:DEBUG>:-O0 -g>
        $<$<CONFIG:RELEASE>:-O3>
)