# ==========================================================
# CompilerOptions.cmake
#
# Configures compiler-specific language features and build
# options for supported toolchains.
#
# Supported:
#   - MSVC
#   - Clang
#   - GCC
#
# This module enables the required C++ language standard,
# compiler conformance settings, and optimization levels for
# different build configurations.
# ==========================================================

function(enable_project_options target)

    target_compile_features(${target} PRIVATE cxx_std_23)

    # MSVC
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")

        target_compile_options(${target} PRIVATE
            /permissive-
            $<$<CONFIG:Debug>:/Od>
            $<$<CONFIG:Release>:/O2>
        )

    # Clang / GNU
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")

        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Debug>:-O0>
            $<$<CONFIG:Release>:-O2>
        )

    endif()

endfunction(enable_project_options)