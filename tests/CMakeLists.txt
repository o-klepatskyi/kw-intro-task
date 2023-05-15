include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)
message(test_deps ${test_deps})
function(add_test_file TEST_NAME TEST_FILE)
    add_executable(${TEST_NAME} ${TEST_FILE})
    target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main modern_cpp_examples)
    gtest_discover_tests(${TEST_NAME})
endfunction()

add_test_file(1_test 1_test.cpp)
add_test_file(2_test 2_test.cpp)