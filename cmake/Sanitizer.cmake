add_library(ASanitizer INTERFACE)
if (MSVC)
    target_compile_options(ASanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=address,undefined>)
    target_link_options(ASanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=address,undefined>)
else()
    target_compile_options(ASanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=address,undefined -fno-omit-frame-pointer>)
    target_link_options(ASanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=address,undefined>)
endif()

add_library(TSanitizer INTERFACE)
if (MSVC)
    target_compile_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=thread,undefined>)
    target_link_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=thread,undefined>)
else()
    target_compile_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=thread,undefined -fno-omit-frame-pointer>)
    target_link_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=thread,undefined>)
endif()

function(copy_address_sanitizer_dll target_name)
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