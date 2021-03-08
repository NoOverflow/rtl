cmake_minimum_required(VERSION 3.8)

project(rtl)
include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(sources
    "include/rtl/Option.hpp"
)

add_library(rtl INTERFACE)
target_include_directories(rtl INTERFACE "include")

if(BUILD_TESTING)
    find_package(Criterion)

    if(Criterion_FOUND)
        add_executable(rtl_unit_tests
            "tests/option.cpp"
        )
        target_link_libraries(rtl_unit_tests rtl)
        target_link_libraries(rtl_unit_tests ${CRITERION_LIBRARIES})
        target_include_directories(rtl_unit_tests
            PRIVATE ${CRITERION_INCLUDE_DIRS})

        add_test(
            NAME rtl_tests_build
            COMMAND "${CMAKE_COMMAND}"
                    --build "${CMAKE_BINARY_DIR}"
                    --config "$<CONFIG>"
                    --target rtl_unit_tests
        )
        set_tests_properties(rtl_tests_build
            PROPERTIES FIXTURES_SETUP rtl_tests_fixture)

        add_test(NAME rtl_tests COMMAND rtl_unit_tests)
        set_tests_properties(rtl_tests
            PROPERTIES FIXTURES_REQUIRED rtl_tests_fixture)
    endif()
endif()
