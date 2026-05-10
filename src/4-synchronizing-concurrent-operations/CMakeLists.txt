file(GLOB lib_srcs CONFIGURE_DEPENDS "*.cpp")

if (NOT lib_srcs)
    set(dummy_src "${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp")
    if (NOT EXISTS "${dummy_src}")
        file(WRITE "${dummy_src}" "// Dummy source file to avoid empty library\n")
    endif()
    set(lib_srcs "${dummy_src}")
endif()

add_library(SynchronizingConcurrentOperationsLib STATIC ${lib_srcs})
add_library(CppConcurrency::SynchronizingConcurrentOperations ALIAS SynchronizingConcurrentOperationsLib)

target_include_directories(SynchronizingConcurrentOperationsLib PUBLIC ${PROJECT_SOURCE_DIR}/include/4-synchronizing-concurrent-operations)
