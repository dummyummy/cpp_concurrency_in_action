add_library(Sanitizer INTERFACE)
if (MSVC)
    target_compile_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=address>)
    target_link_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=address>)
else()
    target_compile_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=address -fno-omit-frame-pointer>)
    target_link_options(Sanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>)
endif()

add_library(TSanitizer INTERFACE)
if (MSVC)
    target_compile_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=thread>)
    target_link_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:/fsanitize=thread>)
else()
    target_compile_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=thread -fno-omit-frame-pointer>)
    target_link_options(TSanitizer INTERFACE $<$<CONFIG:Debug>:-fsanitize=thread>)
endif()

function(copy_sanitizer_dll sanitizer_type target_name)
    if(NOT MSVC)
        return()
    endif()
    
    if(${sanitizer_type} STREQUAL "asan")
        set(SANITIZER_LIB_NAME "clang_rt.asan_dynamic-x86_64.dll")
    elseif(${sanitizer_type} STREQUAL "tsan")
        set(SANITIZER_LIB_NAME "clang_rt.tsan_dynamic-x86_64.dll")
    else()
        message(WARNING "Unknown sanitizer type: ${sanitizer_type}")
        return()
    endif()

    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=${SANITIZER_LIB_NAME}
        OUTPUT_VARIABLE SANITIZER_LIB_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    cmake_path(ABSOLUTE_PATH SANITIZER_LIB_PATH NORMALIZE OUTPUT_VARIABLE SANITIZER_LIB_ABSOLUTE_PATH)

    # 3. 检查库是否存在
    if (NOT EXISTS "${SANITIZER_LIB_ABSOLUTE_PATH}")
        message(WARNING "SANITIZER runtime library '${SANITIZER_LIB_NAME}' not found. Path evaluated: ${SANITIZER_LIB_ABSOLUTE_PATH}")
        return()
    endif()

    # 4. 执行拷贝操作 (巧妙利用 Generator Expression)
    # 如果是 Debug 配置，执行 'cmake -E copy_if_different'
    # 如果不是 Debug 配置，执行 'cmake -E true' (静默成功，什么都不做)
    add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E $<IF:$<CONFIG:Debug>,copy_if_different,true>
                "${SANITIZER_LIB_ABSOLUTE_PATH}" 
                "$<TARGET_FILE_DIR:${target_name}>"
        COMMENT "Copying SANITIZER library to $<TARGET_FILE_DIR:${target_name}> (Debug only)"
        COMMAND_EXPAND_LISTS
    )
endfunction()

function(copy_address_sanitizer_dll target_name)
    message(STATUS "Copy ASAN dll to target: ${target_name}")
    copy_sanitizer_dll("asan" ${target_name})
endfunction()

function(copy_thread_sanitizer_dll target_name)
    message(STATUS "Copy TSAN dll to target: ${target_name}")
    copy_sanitizer_dll("tsan" ${target_name})
endfunction()