cmake_minimum_required(VERSION 3.22)
project(OpenQasmWrapper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -g -O2 -fPIC")
	if (APPLE)  # Workaround for MacOS linker issues with GCC since CLT15
		set(CMAKE_CXX_FLAGS "-ld_classic")
        set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
        set(OpenMP_CXX_LIB_NAMES "libomp")
        set(OpenMP_libomp_LIBRARY /usr/local/opt/libomp/lib/libomp.dylib)  # Change path if necessary
    endif()
endif()

add_subdirectory(QasmParserLib)

include_directories("${CMAKE_SOURCE_DIR}/QasmParserLib/includes")
include_directories("${CMAKE_SOURCE_DIR}/PythonWrapper")

find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(openqasmparser
	"${CMAKE_CURRENT_SOURCE_DIR}/QasmParserLib/src/parser.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/QasmParserLib/includes/parser.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PythonWrapper/pybind11_wrapper.cpp"
)

find_package(fmt REQUIRED)
find_package(TBB REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)
target_link_libraries(openqasmparser PUBLIC fmt::fmt TBB::tbb Threads::Threads OpenMP::OpenMP_CXX)

install(TARGETS openqasmparser
		COMPONENT python
		LIBRARY DESTINATION "${PYTHON_LIBRARY_DIR}"
)
