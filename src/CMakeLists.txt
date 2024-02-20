add_library(flang
        flang/token.cpp
        flang/tokenizer.cpp
        flang/pp/print_token.cpp
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

add_executable(flang-interpreter
        main.cpp)

target_link_libraries(flang-interpreter PRIVATE flang)