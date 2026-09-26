cmake_minimum_required(VERSION 4.3.3)

project(Vector-Internals LANGUAGES CXX)

# ==========================================================
# Project Options
# ==========================================================

option(
    ENABLE_ASAN
    "Enable Address Sanitizer"
    ON
)

option(
    ENABLE_UBSAN
    "Enable Undefined Behaviour Sanitizer"
    OFF
)

option(
    ENABLE_WARNINGS_AS_ERRORS
    "Treat Compiler Warnings As Errors"
    OFF
)

# ==========================================================
# Build Configuration
# ==========================================================

include(cmake/CompilerWarnings.cmake)
include(cmake/CompilerOptions.cmake)
include(cmake/Sanitizers.cmake)

add_executable(Vector-Internals
    include/Memory/Allocator.h
    include/Memory/AllocatorPolicy.h
    include/Memory/AllocationStats.h
    include/Memory/BufferStorage.h
    include/Memory/TrackingAllocator.h
    include/Memory/AllocationRegistry.h
    include/Memory/AllocatorFactory.h
    include/Memory/StatefulAllocator.h
    include/Iterator/IteratorBase.h
    include/Containers/Vector.h
    include/Containers/Vector.tpp

    src/Vector-Internals.cpp
)

target_include_directories(Vector-Internals
    PRIVATE
        include
)

# ==========================================================
# enable public interfaces
# ==========================================================

enable_warnings(Vector-Internals)
enable_project_options(Vector-Internals)
enable_sanitizers(Vector-Internals)


include(GNUInstallDirs)
install(TARGETS Vector-Internals
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ==========================================================
# Tests
# ==========================================================

enable_testing()
add_subdirectory(tests)