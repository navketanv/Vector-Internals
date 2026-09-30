include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

FetchContent_MakeAvailable(googletest)

# ==========================================================
# Target
# ==========================================================
add_executable(VectorInternalsTests
    BufferStorageTests.cpp
    VectorTests.cpp
)

target_include_directories(VectorInternalsTests
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(VectorInternalsTests
    PRIVATE
        GTest::gtest_main
)

enable_project_options(VectorInternalsTests)

include(GoogleTest)

gtest_discover_tests(VectorInternalsTests)