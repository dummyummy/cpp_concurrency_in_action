add_executable(
  CppConcurrencyTests
  custom_gtest_main.cpp
)

target_link_libraries(
  CppConcurrencyTests PRIVATE GTest::gtest TSanitizer
)

if (MSVC)
    copy_thread_sanitizer_dll(CppConcurrencyTests)
endif()

include(GoogleTest)

add_subdirectory(2-thread-management)
add_subdirectory(3-sharing-data-between-threads)
add_subdirectory(4-synchronizing-concurrent-operations)

target_link_libraries(CppConcurrencyTests PRIVATE
  ThreadManagementTests
  SharingDataBetweenThreadsTests
  SynchronizingConcurrentOperationsTests
)

gtest_discover_tests(CppConcurrencyTests)