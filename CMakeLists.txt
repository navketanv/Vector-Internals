cmake_minimum_required(VERSION 4.3.3)

project(Vector-Internals LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Vector-Internals src/Vector-Internals.cpp
    include/Allocator.h
    include/AllocatorPolicy.h
    include/AllocationStats.h
    include/BufferStorage.h
    include/IteratorBase.h
    include/StatefulAllocator.h
    include/Vector.h

    include/Vector.tpp
)

target_include_directories(Vector-Internals
    PRIVATE
        include
)

include(GNUInstallDirs)
install(TARGETS Vector-Internals
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
