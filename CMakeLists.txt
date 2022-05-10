cmake_minimum_required(VERSION 3.1)
project(Shamon C)

include(CTest)

SET (CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
# we need POSIX

OPTION(DYNAMORIO_SOURCES "Build sources based on DynamoRIO" ON)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wpedantic -Werror")
add_compile_options(-Wall -Wextra -pedantic -Werror)
if (${CMAKE_BUILD_TYPE} NOT STREQUAL "Debug")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -flto")
endif()

if (DYNAMORIO_SOURCES)
        find_package(DynamoRIO)
        if (NOT DynamoRIO_FOUND)
	        message(FATAL_ERROR
                        "DynamoRIO package required to build DynamoRIO sources")
        endif(NOT DynamoRIO_FOUND)
endif()

add_library(shamon-utils STATIC utils.c)
target_compile_definitions(shamon-utils PUBLIC -D_POSIX_C_SOURCE=200809L)
add_library(shamon-list OBJECT list.c)
add_library(shamon-vector OBJECT vector.c)
add_library(shamon-queue OBJECT queue.c)
add_library(shamon-parallel-queue OBJECT parallel_queue.c)

add_library(shamon STATIC arbiter.c event.c stream.c)
target_link_libraries(shamon PRIVATE
                      shamon-utils
                      shamon-vector shamon-list
                      shamon-parallel-queue)

add_subdirectory(streams)
add_subdirectory(shmbuf)

if (DYNAMORIO_SOURCES)
       add_subdirectory(drfun)
       #add_subdirectory(drsyscalls)
endif()
# include_directories("${CMAKE_SOURCE_DIR}/shmbuf")
# include_directories("${CMAKE_SOURCE_DIR}/streams")
# include_directories("${CMAKE_SOURCE_DIR}/..")
