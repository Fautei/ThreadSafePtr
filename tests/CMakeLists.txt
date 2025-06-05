include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

add_executable(thread_safe_ptr_test thread_safe_ptr_test.cpp)
target_link_libraries(thread_safe_ptr_test PRIVATE GTest::gtest_main mts)
target_compile_features(thread_safe_ptr_test PRIVATE cxx_std_20)

include(GoogleTest)
gtest_discover_tests(thread_safe_ptr_test)