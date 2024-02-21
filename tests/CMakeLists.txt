enable_testing()

add_executable(unit-tests
        main.cpp
        test_helloworld.cpp
        test_tokenizer.cpp
)

target_link_libraries(unit-tests PRIVATE flang)
target_link_libraries(unit-tests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit-tests)
