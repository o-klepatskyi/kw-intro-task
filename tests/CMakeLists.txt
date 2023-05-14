include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
#add_library(GTest::GTest INTERFACE IMPORTED)
#target_link_libraries(GTest::GTest INTERFACE gtest_main)
enable_testing()
add_executable(multiply_test multiply_test.cpp)
add_executable(add_test add_test.cpp)

target_link_libraries(multiply_test
 PRIVATE
 GTest::gtest_main
 multiply
)

target_link_libraries(add_test
  PRIVATE
  GTest::gtest_main
  multiply
)
include(GoogleTest)
gtest_discover_tests(
  multiply_test
)
gtest_discover_tests(
  add_test
)