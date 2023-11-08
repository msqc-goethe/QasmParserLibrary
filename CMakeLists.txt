cmake_minimum_required(VERSION 3.22)
project(OpenQasmWrapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -g -O2 -ld_classic")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
    set(OpenMP_CXX_LIB_NAMES "libomp")
    set(OpenMP_libomp_LIBRARY /usr/local/opt/libomp/lib/libomp.dylib)
endif()

add_subdirectory(QasmParserLib)
add_subdirectory(pybind11)

include_directories("${CMAKE_SOURCE_DIR}/QasmParserLib/includes")
include_directories("${CMAKE_SOURCE_DIR}/PythonWrapper")

find_package(pybind11 REQUIRED)
pybind11_add_module(openQasmParser
	"QasmParserLib/src/parser.cpp"
	"QasmParserLib/includes/parser.h"
	"PythonWrapper/pybind11_wrapper.cpp"
)

target_link_libraries(openQasmParser PUBLIC)

install(TARGETS openQasmParser
	COMPONENT python
	LIBRARY DESTINATION "${PYTHON_LIBRARY_DIR}"
)
