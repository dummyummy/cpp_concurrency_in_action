add_subdirectory(2-thread-management)
add_subdirectory(3-sharing-data-between-threads)
add_subdirectory(4-synchronizing-concurrent-operations)

add_library(CppConcurrency INTERFACE)

target_link_libraries(CppConcurrency INTERFACE 
    CppConcurrency::ThreadManagement
    CppConcurrency::SharingDataBetweenThreads
    CppConcurrency::SynchronizingConcurrentOperations
)
