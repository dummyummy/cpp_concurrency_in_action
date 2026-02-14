add_library(Sanitizer INTERFACE)
if (MSVC)
    target_compile_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=address>)
    target_link_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=address>)
else()
    target_compile_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>)
    target_link_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>)
endif()

function(copy_sanitizer_dll target_name)
    set(ASAN_DLL_NAME "clang_rt.asan_dynamic-x86_64.dll")

    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${ASAN_DLL_NAME}
        OUTPUT_VARIABLE ASAN_DLL_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    set(ASAN_DLL_FOUND FALSE)

    cmake_path(ABSOLUTE_PATH ASAN_DLL_PATH NORMALIZE OUTPUT_VARIABLE ASAN_DLL_ABSOLUTE_PATH)
    if (EXISTS ${ASAN_DLL_ABSOLUTE_PATH})
        set(ASAN_DLL_FOUND TRUE)
    endif()

    set(SHOULD_ENABLE_ASAN "$<AND:$<CONFIG:Debug>,$<BOOL:${ASAN_DLL_FOUND}>>")

    if (SHOULD_ENABLE_ASAN)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ASAN_DLL_ABSOLUTE_PATH} $<TARGET_FILE_DIR:${target_name}>
            COMMENT "Copying ASAN DLL to $<TARGET_FILE_DIR:${target_name}>"
            COMMAND_EXPAND_LISTS)
    endif()
endfunction()