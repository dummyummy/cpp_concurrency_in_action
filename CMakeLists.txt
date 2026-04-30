cmake_minimum_required(VERSION 3.20)

project(CppConcurrencyInAction LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(Sanitizer)

if(POLICY CMP0091)
    cmake_policy(SET CMP0091 NEW)
endif()
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

set(gtest_enable_tsan ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

set(GTEST_TARGETS gtest)

foreach(_target ${GTEST_TARGETS})
    if(TARGET ${_target})
        if (NOT ${gtest_enable_tsan})
            target_link_libraries(${_target} PRIVATE Sanitizer)
            message(STATUS "Injecting ASan flags into target: ${_target}")
        else()
            target_link_libraries(${_target} PRIVATE TSanitizer)
            message(STATUS "Injecting TSan flags into target: ${_target}")
        endif()
    endif()
endforeach()

enable_testing()

add_subdirectory(src)
add_subdirectory(app)
add_subdirectory(tests)