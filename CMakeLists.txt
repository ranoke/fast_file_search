cmake_minimum_required (VERSION 2.8.11)

set (CMAKE_CXX_STANDARD 23)

project (parse_file)

# add_library(TracyClient STATIC tracy/public/TracyClient.cpp
#                                tracy/public/tracy/TracyOpenCL.hpp)
# target_include_directories(TracyClient PUBLIC tracy/public/tracy)
# target_compile_definitions(TracyClient PUBLIC TRACY_ENABLE=0)

add_executable (parse_file main.cpp)

# target_link_libraries(parse_file TracyClient murmurhash)
target_link_libraries(parse_file murmurhash)
