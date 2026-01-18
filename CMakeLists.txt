cmake_minimum_required(VERSION 3.18)
project(LotusLib)

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
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -O0 -ggdb")
endif()

#######################################################################################
# LotusLib library

file(GLOB LOTUS_LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${LOTUS_LIB_SOURCES} PROPERTIES LANGUAGE CXX)

if(BUILD_SHARED_LIBS)
	add_library(LotusLib SHARED ${LOTUS_LIB_SOURCES})
else()
	add_library(LotusLib STATIC ${LOTUS_LIB_SOURCES})
endif()

target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/Oodle-2.9/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/Binary-Reader/include")

#######################################################################################
# Oodle

if(WIN32)
	find_library(OODLE_CORE REQUIRED NAMES oo2core_win64 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64 ${CMAKE_SOURCE_DIR}/bin/Win64)	
elseif(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Release")
	find_library(OODLE_CORE REQUIRED NAMES oo2corelinux64 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux ${CMAKE_SOURCE_DIR}/bin/Linux)
elseif(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	find_library(OODLE_CORE REQUIRED NAMES oo2corelinux64_debug PATHS ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux ${CMAKE_SOURCE_DIR}/bin/Linux)
endif()

target_link_libraries(LotusLib ${OODLE_CORE})

#######################################################################################
# Libraries

# Json - Parse data inside Packages.bin
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/json")
target_link_libraries(LotusLib nlohmann_json::nlohmann_json)

# zstd - Decompress Packages.bin
set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "Build executable zstd programs")
set(ZSTD_BUILD_TESTS OFF CACHE BOOL "Build zstd test suite")
set(ZSTD_BUILD_CONTRIB OFF CACHE BOOL "Build zstd contribution documentation")
set(ZSTD_BUILD_COMPRESSION ON CACHE BOOL "Build zstd compression module")
set(ZSTD_BUILD_DEPRECATED ON CACHE BOOL "no")
set(ZSTD_BUILD_DECOMPRESSION ON CACHE BOOL "Build zstd decompression module")
set(ZSTD_BUILD_DICTBUILDER ON CACHE BOOL "Build zstd dict builder")

if (BUILD_SHARED_LIBS)
	set(ZSTD_BUILD_STATIC OFF CACHE BOOL "")
	set(ZSTD_BUILD_SHARED ON  CACHE BOOL "")
	set(ZSTD_BUILD_SHARED_LIBS ON CACHE BOOL "")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/zstd/build/cmake")
	target_link_libraries(LotusLib libzstd_shared)
else()
	set(ZSTD_BUILD_STATIC ON  CACHE BOOL "")
	set(ZSTD_BUILD_SHARED OFF CACHE BOOL "")
	set(ZSTD_BUILD_SHARED_LIBS OFF CACHE BOOL "")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/zstd/build/cmake")
	target_link_libraries(LotusLib libzstd_static)
endif()

# lzf - Decompress pre-ensmallening data inside .cache files
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/liblzf/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/liblzf")
target_link_libraries(LotusLib lzf)

# spdlog - Logging 
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog")
target_link_libraries(LotusLib spdlog)
