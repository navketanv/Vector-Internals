cmake_minimum_required(VERSION 4.3.3)

project(Vector-Internals LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Vector-Internals src/Vector-Internals.cpp
    include/Vector.h include/Vector.tpp
    include/Allocator.h include/Allocator.tpp
    include/BufferStorage.h include/BufferStorage.tpp


    include/IteratorBase.h
    include/AllocatorPolicy.h
)

include(GNUInstallDirs)
install(TARGETS Vector-Internals
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

target_sources(Vector-Internals PRIVATE include/Allocator.tpp include/BufferStorage.tpp include/Vector.tpp)
