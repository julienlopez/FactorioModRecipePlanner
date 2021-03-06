cmake_minimum_required(VERSION 3.2)
project(FactorioModRecipePlanner)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions("-DNOMINMAX")
add_definitions("-std=c++17")

if(ENABLE_COVERAGE STREQUAL "ON")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage -O0")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage -fprofile-arcs")
endif()

include_directories(.)
include_directories(3rd_party)
include_directories(3rd_party/GSL/include)

file(GLOB_RECURSE SOURCE_FILES ./src/*)
add_executable(FactorioModRecipePlanner ${SOURCE_FILES} 3rd_party/json/json.cc)
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	# using GCC
	target_link_libraries(FactorioModRecipePlanner stdc++fs)  # link to libstdc++fs
endif()