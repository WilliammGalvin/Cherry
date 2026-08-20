cmake_minimum_required(VERSION 3.28)

project(cherry
    VERSION 0.1.0
    DESCRIPTION "The Cherry and Pit languages"
    LANGUAGES CXX
)

if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_CXX_STANDARD 23)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
  endif()

  include(CTest)
endif()

option(CHERRY_BUILD_TESTS "Build the test suite" ${PROJECT_IS_TOP_LEVEL})
option(CHERRY_BUILD_TOOLS "Build the pit and cherry drivers" ON)
option(CHERRY_WERROR "Treat compiler warnings as errors" OFF)

# Every first-party header is included as "common/...", "pit/...", "cherry/..."
# relative to this root, so the monorepo layout is visible at each include site.
set(CHERRY_INCLUDE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

include(cmake/warnings.cmake)
include(cmake/component.cmake)

add_subdirectory(src)

if(CHERRY_BUILD_TOOLS)
  add_subdirectory(tools)
endif()

if(CHERRY_BUILD_TESTS AND BUILD_TESTING)
  add_subdirectory(tests)
endif()
