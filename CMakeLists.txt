cmake_minimum_required(VERSION 3.18)
project(LotusLib)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /W4")
elseif(UNIX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")
endif()

# Objects
file(GLOB LOTUS_LIB_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set_source_files_properties(${LOTUS_LIB_SOURCES} PROPERTIES LANGUAGE CXX)
add_library(LotusLib STATIC ${LOTUS_LIB_SOURCES})

# Shared Library
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/Oodle-2.9/include")
target_include_directories(LotusLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include")
