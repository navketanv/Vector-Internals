cmake_minimum_required(VERSION 4.3.3)

project(Vector-Internals LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Vector-Internals src/Vector-Internals.cpp)

include(GNUInstallDirs)
install(TARGETS Vector-Internals
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
