cmake_minimum_required(VERSION 3.20)
project(vyshkat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(VYSHKAT_COVERAGE "Instrument project code for GNU gcov" OFF)
if(VYSHKAT_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "Coverage configuration currently supports GCC only")
    endif()
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
endif()

add_library(vyshkat_domain
    src/transport.cpp
    src/service_center.cpp
    src/inventory.cpp
    src/items.cpp
    src/park.cpp
)
    
target_include_directories(vyshkat_domain PUBLIC src)

add_library(vyshkat_console src/console_app.cpp)
target_link_libraries(vyshkat_console PUBLIC vyshkat_domain)

add_executable(vyshkat src/main.cpp)
target_link_libraries(vyshkat PRIVATE vyshkat_console)
target_include_directories(vyshkat SYSTEM PRIVATE third_party/boost-di)
file(SHA256 "${CMAKE_CURRENT_SOURCE_DIR}/third_party/boost-di/di.hpp" BOOST_DI_SHA256)
if(NOT BOOST_DI_SHA256 STREQUAL "1680ca33ffa04457edcea5c14346ad6e3138d8bb34d4fe074a59e13e34b54827")
    message(FATAL_ERROR "Vendored Boost.Ext.DI header checksum mismatch")
endif()

foreach(target vyshkat_domain vyshkat_console vyshkat)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4 /utf-8)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endforeach()

include(CTest)
if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    add_executable(vyshkat_tests
        tests/transport_tests.cpp
        tests/service_center_tests.cpp
        tests/inventory_tests.cpp
        tests/park_tests.cpp
        tests/energy_total_tests.cpp
        tests/console_tests.cpp
    )
    target_link_libraries(vyshkat_tests PRIVATE vyshkat_console GTest::gtest_main)
    if(MSVC)
        target_compile_options(vyshkat_tests PRIVATE /W4 /utf-8)
    else()
        target_compile_options(vyshkat_tests PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    include(GoogleTest)
    gtest_discover_tests(vyshkat_tests)

    find_package(Python3 3.9 COMPONENTS Interpreter QUIET)
    if(Python3_Interpreter_FOUND)
        add_test(NAME CliSmoke COMMAND "${Python3_EXECUTABLE}"
            "${CMAKE_CURRENT_SOURCE_DIR}/scripts/smoke.py" $<TARGET_FILE:vyshkat>)
    endif()
endif()
