find_package(GTest QUIET)

if (NOT GTest_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        gtest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )

    FetchContent_MakeAvailable(gtest)
endif ()