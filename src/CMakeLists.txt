file(GLOB_RECURSE SOURCES LIST_DIRECTORIES true *.h *.cpp)

add_library(modern_cpp_examples ${SOURCES})

target_include_directories(modern_cpp_examples PUBLIC include)
