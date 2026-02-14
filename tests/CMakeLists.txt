add_executable(
  CppConcurrencyTests
  custom_gtest_main.cpp
  stack_use_after_scope.cpp
)

target_link_libraries(
  CppConcurrencyTests PRIVATE CppConcurrency GTest::gtest Sanitizer
)

# add_executable(
#   AsanVerification
#   asan_verification.cpp
# )

# target_link_libraries(
#   AsanVerification PRIVATE Sanitizer
# )

copy_sanitizer_dll(CppConcurrencyTests)
# copy_sanitizer_dll(AsanVerification)

include(GoogleTest)
gtest_discover_tests(CppConcurrencyTests)
