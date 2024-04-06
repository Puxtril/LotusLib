cmake_minimum_required(VERSION 3.18)
project(LotusLib)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
	message(STATUS "Setting build type to Release.")
	message(STATUS "To Debug, run `cmake -DCMAKE_BUILD_TYPE=Debug`")
	set(CMAKE_BUILD_TYPE "Release")
endif()

if(MSVC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} /O2 /W4")
elseif(UNIX)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall -ggdb")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -O0 -ggdb")
endif()

# ZSTD Build Options (Off)
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "Build executable zstd programs")
set(ZSTD_BUILD_TESTS OFF CACHE BOOL "Build zstd test suite")
set(ZSTD_BUILD_CONTRIB OFF CACHE BOOL "Build zstd contribution documentation")
# ZSTD Build modules (Off)
set(ZSTD_BUILD_SHARED OFF CACHE BOOL "Build std shared libraries")
set(ZSTD_BUILD_COMPRESSION OFF CACHE BOOL "Build zstd compression module")
set(ZSTD_BUILD_DEPRECATED OFF CACHE BOOL "no")
# ZSTD Build modules (On)
set(ZSTD_BUILD_DECOMPRESSION ON CACHE BOOL "Build zstd decompression module")
set(ZSTD_BUILD_STATIC ON CACHE BOOL "Build zstd static libraries")
set(ZSTD_BUILD_DICTBUILDER ON CACHE BOOL "Build zstd dict builder")
# ZSTD library
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/zstd/build/cmake")

# Objects
file(GLOB LOTUS_LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${LOTUS_LIB_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(LotusLib STATIC ${LOTUS_LIB_SOURCES})

# Static Library
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/Oodle-2.9/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/Binary-Reader/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/zstd/lib/decompress")
