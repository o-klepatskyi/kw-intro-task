include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)
enable_testing()
include(GoogleTest)

add_executable(modern_cpp_test ${TEST_SOURCES})
target_link_libraries(modern_cpp_test PRIVATE gtest modern_cpp_examples)
gtest_discover_tests(modern_cpp_test)