file(GLOB test_srcs CONFIGURE_DEPENDS "*.cpp")

add_library(ThreadManagementTests OBJECT ${test_srcs})

target_link_libraries(ThreadManagementTests PUBLIC CppConcurrency PRIVATE GTest::gtest Sanitizer)
