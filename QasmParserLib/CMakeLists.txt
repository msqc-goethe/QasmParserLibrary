cmake_minimum_required(VERSION 3.22)
project(OpenQasmWrapper)

include(FetchContent)

# Generate 'qasmParserLib' library
add_library(qasmParserLib SHARED src/parser.cpp)

target_include_directories(qasmParserLib PUBLIC includes)

FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG master)

FetchContent_MakeAvailable(fmt)
target_link_libraries(qasmParserLib PUBLIC fmt::fmt)

find_package(TBB REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

target_link_libraries(qasmParserLib PUBLIC TBB::tbb Threads::Threads OpenMP::OpenMP_CXX)
