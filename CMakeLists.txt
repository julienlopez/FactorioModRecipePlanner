cmake_minimum_required(VERSION 3.2)
project(FactorioModRecipePlanner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions("-DNOMINMAX")

if(ENABLE_COVERAGE STREQUAL "ON")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage -O0")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -fprofile-arcs")
endif()

include_directories(.)
include_directories(3rd_party/range)

#include(cmake/boost.cmake)
#include(cmake/gsl.cmake)

file(GLOB_RECURSE SOURCE_FILES ./*)
add_executable(FactorioModRecipePlanner ${SOURCE_FILES})
