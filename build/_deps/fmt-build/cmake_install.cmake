# Install script for directory: /Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-build/libfmt.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfmt.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfmt.a")
    execute_process(COMMAND "/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libfmt.a")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/fmt" TYPE FILE FILES
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/args.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/chrono.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/color.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/compile.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/core.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/format.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/format-inl.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/os.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/ostream.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/printf.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/ranges.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/std.h"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-src/include/fmt/xchar.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-build/fmt-config.cmake"
    "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-build/fmt-config-version.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake"
         "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt/fmt-targets.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/fmt" TYPE FILE FILES "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-build/CMakeFiles/Export/lib/cmake/fmt/fmt-targets-noconfig.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/Users/cedricgaberle/CLionProjects/QasmParser/Library/build/_deps/fmt-build/fmt.pc")
endif()

