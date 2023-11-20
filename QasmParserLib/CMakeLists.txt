cmake_minimum_required(VERSION 3.22)
project(OpenQasmWrapper)

# Generate 'qasmParserLib' library
add_library(qasmParserLib SHARED src/parser.cpp)

#if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
#    set(CMAKE_CXX_FLAGS "-Wall -g -O2")
#
#    if(APPLE)
#        set(CMAKE_CXX_FLAGS "-ld_classic")
#    endif ()
#endif ()

target_include_directories(qasmParserLib PUBLIC includes)

find_package(fmt REQUIRED)
find_package(TBB REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

if (TBB_FOUND)
    target_link_libraries(qasmParserLib PUBLIC fmt::fmt TBB::tbb Threads::Threads OpenMP::OpenMP_CXX)
endif ()

