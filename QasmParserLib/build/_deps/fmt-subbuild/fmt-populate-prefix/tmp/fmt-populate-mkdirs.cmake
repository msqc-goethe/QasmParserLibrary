# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-src"
  "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-build"
  "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-subbuild/fmt-populate-prefix"
  "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-subbuild/fmt-populate-prefix/tmp"
  "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-subbuild/fmt-populate-prefix/src/fmt-populate-stamp"
  "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-subbuild/fmt-populate-prefix/src"
  "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-subbuild/fmt-populate-prefix/src/fmt-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-subbuild/fmt-populate-prefix/src/fmt-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/cedricgaberle/CLionProjects/QasmParser/Library/QasmParserLib/build/_deps/fmt-subbuild/fmt-populate-prefix/src/fmt-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
