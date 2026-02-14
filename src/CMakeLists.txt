add_subdirectory(2-thread-management)

add_library(CppConcurrency INTERFACE)

target_link_libraries(CppConcurrency INTERFACE CppConcurrency::ThreadManagement)