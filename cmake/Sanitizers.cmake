# ==========================================================
# Sanitizers.cmake
#
# Enables compiler/runtime sanitizers for supported compilers.
#
# Supported:
#   - Address Sanitizer (ASan)
#   - Undefined Behavior Sanitizer (UBSan)
#
# Sanitizers are enabled only for Debug builds.
# ==========================================================

function(enable_sanitizers target)

    # MSVC
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        # Address Sanitizer
        if (ENABLE_ASAN)

            target_compile_options(${target} PRIVATE
                $<$<CONFIG:Debug>:/fsanitize=address>
            )

            target_link_options(${target} PRIVATE
                $<$<CONFIG:Debug>:/fsanitize=address>
            )

        endif()

    # Clang / GNU
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")

        # Address Sanitizer
        if (ENABLE_ASAN)

            target_compile_options(${target} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address>
                $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
            )

            target_link_options(${target} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=address>
            )

        endif()

        # Undefined Behavior Sanitizer
        if (ENABLE_UBSAN)

            target_compile_options(${target} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=undefined>
            )

            target_link_options(${target} PRIVATE
                $<$<CONFIG:Debug>:-fsanitize=undefined>
            )

        endif()

    endif()

endfunction(enable_sanitizers)
