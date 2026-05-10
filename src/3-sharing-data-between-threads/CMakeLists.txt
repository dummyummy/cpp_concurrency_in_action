file(GLOB lib_srcs CONFIGURE_DEPENDS "*.cpp")

if (NOT lib_srcs)
    set(dummy_src "${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp")
    if (NOT EXISTS "${dummy_src}")
        file(WRITE "${dummy_src}" "// Dummy source file to avoid empty library\n")
    endif()
    set(lib_srcs "${dummy_src}")
endif()

add_library(SharingDataBetweenThreadsLib STATIC ${lib_srcs})
add_library(CppConcurrency::SharingDataBetweenThreads ALIAS SharingDataBetweenThreadsLib)

target_include_directories(SharingDataBetweenThreadsLib PUBLIC ${PROJECT_SOURCE_DIR}/include/3-sharing-data-between-threads)
