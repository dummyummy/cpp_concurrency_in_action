add_executable(
  CppConcurrencyTests
  custom_gtest_main.cpp
)

target_link_libraries(
  CppConcurrencyTests PRIVATE GTest::gtest Sanitizer
)
copy_sanitizer_dll(CppConcurrencyTests)

include(GoogleTest)

add_subdirectory(2-thread-management)
add_subdirectory(3-sharing-data-between-threads)

target_link_libraries(CppConcurrencyTests PRIVATE
  ThreadManagementTests
  SharingDataBetweenThreadsTests
)

gtest_discover_tests(CppConcurrencyTests)