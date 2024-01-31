cmake_minimum_required(VERSION 3.22)
project(OpenQasmWrapper)

# Generate 'qasmParserLib' library
add_library(qasmParserLib SHARED src/parser.cpp)

target_include_directories(qasmParserLib PUBLIC includes)

find_package(TBB REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)
find_package(fmt REQUIRED)

target_link_libraries(qasmParserLib PUBLIC TBB::tbb Threads::Threads OpenMP::OpenMP_CXX fmt::fmt)
