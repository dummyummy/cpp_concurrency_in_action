file(GLOB lib_srcs CONFIGURE_DEPENDS "*.cpp")

add_library(ThreadManagementLib STATIC ${lib_srcs})
add_library(CppConcurrency::ThreadManagement ALIAS ThreadManagementLib)

target_include_directories(ThreadManagementLib PUBLIC ${PROJECT_SOURCE_DIR}/include/2-thread-management)