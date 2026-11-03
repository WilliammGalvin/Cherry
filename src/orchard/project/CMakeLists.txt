add_library(orchard_project project.cpp)
add_library(orchard::project ALIAS orchard_project)

target_sources(orchard_project PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
    project.hpp
    module.hpp
    source_file.hpp
)

target_link_libraries(orchard_project PRIVATE cherry::options)

add_subdirectory(debug)
