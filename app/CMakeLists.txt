add_executable(MainApp main.cpp)

if (MSVC)
    target_compile_options(MainApp PRIVATE /W4
        $<$<CONFIG:Debug>:/Zi /Od /Oy->)
else()
    target_compile_options(MainApp PRIVATE -Wall -Wextra -Wpedantic
        $<$<CONFIG:Debug>:-g -O0>)
endif()

target_link_libraries(MainApp PRIVATE CppConcurrency)
