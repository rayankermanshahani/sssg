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
target_compile_options(sssg PRIVATE -Wall -Wextra -Wpedantic)

# compiler optimizations
if (CMAKE_BUILD_TYPE MATCHES Release)
  target_compile_options(sssg PRIVATE -O3 -march=native -ffast-math -flto)
  target_link_options(sssg PRIVATE -flto)
else()
  # debug build settings
  target_compile_options(sssg PRIVATE -g)
endif()

# installation configuration
include(GNUInstallDirs)
install(TARGETS sssg 
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT sssg_runtime)
