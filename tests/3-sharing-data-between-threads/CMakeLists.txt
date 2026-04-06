file(GLOB test_srcs CONFIGURE_DEPENDS "*.cpp")

if (NOT test_srcs)
    set(dummy_src "${CMAKE_CURRENT_BINARY_DIR}/dummy.cpp")
    if (NOT EXISTS "${dummy_src}")
        file(WRITE "${dummy_src}" "// Dummy source file to avoid empty library\n")
    endif()
    set(test_srcs "${dummy_src}")
endif()

add_library(SharingDataBetweenThreadsTests OBJECT ${test_srcs})

target_link_libraries(SharingDataBetweenThreadsTests PUBLIC CppConcurrency PRIVATE GTest::gtest Sanitizer)
