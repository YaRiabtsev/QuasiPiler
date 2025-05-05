cmake_minimum_required(VERSION 3.29)

project(qpiler VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Og -g -fno-omit-frame-pointer
            -Werror

            -Wall
            -Wextra
            -Wpedantic

            -Wcast-align
            -Wcast-qual
            -Wconversion
            -Wctor-dtor-privacy
            -Wenum-compare
            -Wfloat-equal
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Woverloaded-virtual
            -Wredundant-decls
            -Wsign-conversion
            -Wsign-promo
    )
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -mtune=native -DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s")
endif ()

add_library(qpiler_lib
        src/reader.cpp
        src/lexer.cpp
)

target_include_directories(qpiler_lib PUBLIC include)

target_precompile_headers(qpiler_lib PRIVATE
        include/reader.hpp
        include/lexer.hpp
)

set_target_properties(qpiler_lib PROPERTIES UNITY_BUILD ON)

add_executable(qpiler src/main.cpp)

target_link_libraries(qpiler PRIVATE qpiler_lib)

option(BUILD_TESTS "Build unit tests" ON)
option(COVERAGE "Enable coverage reporting" OFF)

if (BUILD_TESTS)
    find_package(GTest REQUIRED)

    if (UNIX)
        find_package(Threads REQUIRED)
    endif ()

    add_executable(unit_tests
            tests/main.cpp
            tests/reader_tests.cpp
            tests/lexer_tests.cpp
    )

    target_link_libraries(unit_tests
            qpiler_lib
            GTest::GTest
            GTest::Main
            Threads::Threads
    )

    enable_testing()
    add_test(NAME unit_tests COMMAND unit_tests)

    set(SOURCE_COMPILERS_DIR "${CMAKE_SOURCE_DIR}/tests/data")
    set(TARGET_COMPILERS_DIR "${CMAKE_BINARY_DIR}/test_data")

    if (NOT EXISTS ${TARGET_COMPILERS_DIR})
        if (UNIX)
            message(STATUS "Creating symlink for QPiler test files from ${SOURCE_COMPILERS_DIR} to ${TARGET_COMPILERS_DIR}")
            execute_process(
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${SOURCE_COMPILERS_DIR} ${TARGET_COMPILERS_DIR}
            )
        else ()
            message(STATUS "Copying QPiler test files from ${SOURCE_COMPILERS_DIR} to ${TARGET_COMPILERS_DIR}")
            execute_process(
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_COMPILERS_DIR} ${TARGET_COMPILERS_DIR}
            )
        endif ()
    else ()
        message(STATUS "Symlink or test data directory already exists")
    endif ()

    if (COVERAGE)
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            message(STATUS "Enabling coverage reporting")
            target_compile_options(qpiler_lib PUBLIC --coverage -O0)
            target_link_libraries(qpiler_lib gcov)
            target_compile_options(unit_tests PUBLIC --coverage -O0)
            target_link_libraries(unit_tests gcov)
        else ()
            message(WARNING "Coverage is only supported with GCC or Clang compilers")
        endif ()
    endif ()
endif ()

#if (BUILD_TESTS AND CMAKE_BUILD_TYPE STREQUAL "Debug")
#    target_compile_options(unit_tests PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
#    target_link_options(unit_tests PRIVATE -fsanitize=address)
#endif()