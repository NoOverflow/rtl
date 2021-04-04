cmake_minimum_required(VERSION 3.11)

if(TARGET rtl)
    return()
endif()

project(rtl)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME})
	option(PACKAGE_TESTS "Build the tests" ON)
else()
	option(PACKAGE_TESTS "Build the tests" OFF)
endif()

set(sources
    "include/rtl/Option.hpp"
)

add_library(rtl INTERFACE)
target_include_directories(rtl INTERFACE "include")

if(PACKAGE_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
