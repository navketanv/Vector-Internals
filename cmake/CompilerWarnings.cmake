# ==========================================================
# CompilerWarnings.cmake
#
# Configures compiler warning levels and diagnostics for the
# supported toolchains.
#
# Supported:
#   - MSVC
#   - Clang
#   - GCC
#
# The selected warnings promote modern C++ practices while
# keeping the diagnostic output practical for day-to-day
# development.
# ==========================================================

function(enable_warnings target)

    # MSVC
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
        )

        if (ENABLE_WARNINGS_AS_ERRORS)
            target_compile_options(${target} PRIVATE
                /WX
            )
        endif()

    # Clang / GNU
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")

        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wshadow
            -Wconversion
            -Wold-style-cast
            -Wnon-virtual-dtor
            -Wnull-dereference
        )

        if (ENABLE_WARNINGS_AS_ERROR)
            target_compile_options(${target} PRIVATE
                -Werror
            )
        endif()

    endif()

endfunction(enable_warnings)