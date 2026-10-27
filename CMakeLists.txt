cmake_minimum_required(VERSION 3.28)

project(cherry
    VERSION 0.1.0
    DESCRIPTION "Cherry monorepo"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/options.cmake)

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(tests)

