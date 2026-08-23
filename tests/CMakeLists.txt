include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
    EXCLUDE_FROM_ALL
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

set(CHERRY_FIXTURE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/fixtures")

function(cherry_add_test name)
  cmake_parse_arguments(ARG "" "" "SOURCES;DEPENDS" ${ARGN})

  add_executable(${name} ${ARG_SOURCES})
  target_link_libraries(${name}
        PRIVATE
            ${ARG_DEPENDS}
            cherry::warnings
            GTest::gtest_main
    )
  target_compile_definitions(${name}
        PRIVATE CHERRY_FIXTURE_ROOT="${CHERRY_FIXTURE_ROOT}"
    )
  gtest_discover_tests(${name})
endfunction()

add_subdirectory(unit)
