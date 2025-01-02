cmake_minimum_required(VERSION 3.10)

# project info
project(sssg VERSION 1.0 DESCRIPTION "A Simple Static Site Generator" LANGUAGES CXX)

# c++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# source code files
add_executable(sssg
  src/main.cpp
  src/markdown.cpp
)

# header files
target_include_directories(sssg PRIVATE src)

# compiler warnings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  target_compile_options(sssg PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(sssg PRIVATE /W4)
endif()

# compiler optimizations
if (CMAKE_BUILD_TYPE MATCHES Release)
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(sssg PRIVATE -O3 -march=native -ffast-math -flto)
    target_link_options(sssg PRIVATE -flto)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sssg PRIVATE /O2 /GL)
    target_link_options(sssg PRIVATE /LTCG)
  endif()
else()
  # debug build settings
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(sssg PRIVATE -g)
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(sssg PRIVATE /DEBUG)
  endif()
endif()

# optional: add install rules
install(TARGETS sssg DESTINATION bin)

