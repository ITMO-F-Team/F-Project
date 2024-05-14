add_library(flang
        flang/pp/ast_printer.cpp
        flang/tokenize/token.cpp
        flang/tokenize/tokenizer.cpp
        flang/parse/parser_impl.cpp
        flang/parse/parser.cpp
        flang/eval/environment_stack.cpp
        flang/eval/eval_visitor.cpp
)

target_include_directories(
        flang
        SYSTEM
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
)

target_compile_options(flang
        INTERFACE
        $<$<CONFIG:DEBUG>:-Wall -Werror -Wextra-semi -O1 -g>
        PRIVATE
        $<$<CONFIG:RELEASE>:-O3>
)

# target_link_libraries(flang PRIVATE nlohmann_json::nlohmann_json)

add_executable(flang-interpreter
        main.cpp)

target_link_libraries(flang-interpreter PRIVATE flang)
