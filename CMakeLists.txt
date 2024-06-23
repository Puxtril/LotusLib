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
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -Wall")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -Wall -O0 -ggdb")
endif()

#######################################################################################
# Add LotusLib library

file(GLOB LOTUS_LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${LOTUS_LIB_SOURCES} PROPERTIES LANGUAGE CXX)

add_library(LotusLib STATIC ${LOTUS_LIB_SOURCES})

target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/Oodle-2.9/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/Binary-Reader/include")

#######################################################################################
# Oodle

# These can be placed in the LotusLib folder or the main project folder 
if(WIN32)
	find_library(OODLE_CORE REQUIRED NAMES oo2core_win64 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/bin/Win64 ${CMAKE_SOURCE_DIR}/bin/Win64)	
elseif(APPLE)
	find_library(OODLE_CORE REQUIRED NAMES oo2core64 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/bin/Mac ${CMAKE_SOURCE_DIR}/bin/Mac)
elseif(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Release")
	find_library(OODLE_CORE REQUIRED NAMES oo2corelinux64 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux ${CMAKE_SOURCE_DIR}/bin/Linux)
elseif(UNIX AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	find_library(OODLE_CORE REQUIRED NAMES oo2corelinux64_debug PATHS ${CMAKE_CURRENT_SOURCE_DIR}/bin/Linux ${CMAKE_SOURCE_DIR}/bin/Linux)
endif()

target_link_libraries(LotusLib ${OODLE_CORE})

#######################################################################################
# Json - Needed to parse data inside Packages.bin

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/json")

target_link_libraries(LotusLib nlohmann_json::nlohmann_json)

#######################################################################################
# zstd - Needed for decompressing Packages.bin

option(LOTUSLIB_BUILD_ZSTD_STATIC "Build zstd statically" ON)
option(LOTUSLIB_BUILD_ZSTD_DYNAMIC "Build zstd dynamically" OFF)

if (LOTUSLIB_BUILD_ZSTD_STATIC AND LOTUSLIB_BUILD_ZSTD_DYNAMIC)
	message(FATAL_ERROR "Only set 1 of these: LOTUSLIB_BUILD_ZSTD_STATIC LOTUSLIB_BUILD_ZSTD_DYNAMIC")
endif()

if (LOTUSLIB_BUILD_ZSTD_STATIC)
	message(STATUS "Building zstd statically")
	set(ZSTD_BUILD_STATIC ON  CACHE BOOL "Build zstd static libraries")
	set(ZSTD_BUILD_SHARED OFF CACHE BOOL "Build zstd shared libraries")
	set(ZSTD_BUILD_SHARED_LIBS OFF CACHE BOOL "Build zstd shared libraries")
endif()

if (LOTUSLIB_BUILD_ZSTD_DYNAMIC)
	message(STATUS "Building zstd dynamically")
	set(ZSTD_BUILD_STATIC OFF CACHE BOOL "Build zstd static libraries")
	set(ZSTD_BUILD_SHARED ON  CACHE BOOL "Build zstd shared libraries")
	set(ZSTD_BUILD_SHARED_LIBS ON CACHE BOOL "Build zstd shared libraries")
endif()

if (LOTUSLIB_BUILD_ZSTD_STATIC OR LOTUSLIB_BUILD_ZSTD_DYNAMIC)
	# ZSTD Build Options (Off)
	set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL "Build executable zstd programs")
	set(ZSTD_BUILD_TESTS OFF CACHE BOOL "Build zstd test suite")
	set(ZSTD_BUILD_CONTRIB OFF CACHE BOOL "Build zstd contribution documentation")
	
	# ZSTD Build modules (Off)
	set(ZSTD_BUILD_COMPRESSION ON CACHE BOOL "Build zstd compression module")
	set(ZSTD_BUILD_DEPRECATED ON CACHE BOOL "no")
	
	# ZSTD Build modules (On)
	set(ZSTD_BUILD_DECOMPRESSION ON CACHE BOOL "Build zstd decompression module")
	set(ZSTD_BUILD_DICTBUILDER ON CACHE BOOL "Build zstd dict builder")
	
	# ZSTD library
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/zstd/build/cmake")

	if (LOTUSLIB_BUILD_ZSTD_STATIC)
		target_link_libraries(LotusLib libzstd_static)
	elseif(LOTUSLIB_BUILD_ZSTD_DYNAMIC)
		target_link_libraries(LotusLib libzstd_shared)
	endif()

# TODO: Write FindZSTD.cmake to find zstd installed by system (linux package manager)
else()
	message(FATAL_ERROR "Neither LOTUSLIB_BUILD_ZSTD_STATIC nor LOTUSLIB_BUILD_ZSTD_DYNAMIC is set")
endif()

#######################################################################################
# lzf - Needed for decompressing some (old) data inside .cache files

target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/liblzf/include")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/liblzf")
target_link_libraries(LotusLib lzf)

#######################################################################################
# spdlog

option(LOTUSLIB_BUILD_SPDLOG_STATIC "Build spdlog statically" ON)
option(LOTUSLIB_BUILD_SPDLOG_DYNAMIC "Build spdlog dynamically" OFF)

if (LOTUSLIB_BUILD_SPDLOG_STATIC AND LOTUSLIB_BUILD_SPDLOG_DYNAMIC)
	message(FATAL_ERROR "Only set 1 of these: LOTUSLIB_BUILD_SPDLOG_STATIC LOTUSLIB_BUILD_SPDLOG_DYNAMIC")
endif()

if (LOTUSLIB_BUILD_SPDLOG_STATIC)
	message(STATUS "Building spdlog statically")
	set(SPDLOG_BUILD_SHARED      OFF CACHE INTERNAL "")
	set(SPDLOG_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
endif()

if (LOTUSLIB_BUILD_SPDLOG_DYNAMIC)
	message(STATUS "Building spdlog dynamically")
	set(SPDLOG_BUILD_SHARED      ON CACHE INTERNAL "")
	set(SPDLOG_BUILD_SHARED_LIBS ON CACHE INTERNAL "")
endif()

if (LOTUSLIB_BUILD_SPDLOG_STATIC OR LOTUSLIB_BUILD_SPDLOG_DYNAMIC)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog")
	target_link_libraries(LotusLib spdlog)

# TODO: Write FindSPDLOG.cmake to find spdlog installed by system (linux package manager)
else()
	message(FATAL_ERROR "Neither LOTUSLIB_BUILD_SPDLOG_STATIC nor LOTUSLIB_BUILD_SPDLOG_DYNAMIC is set")
endif()
