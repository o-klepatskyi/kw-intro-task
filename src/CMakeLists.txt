file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_library(modern_cpp_examples ${SOURCES})
add_executable(modern_cpp_examples_run ${SOURCES})
target_include_directories(modern_cpp_examples PUBLIC include)
target_include_directories(modern_cpp_examples_run PUBLIC include)
