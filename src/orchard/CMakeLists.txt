add_subdirectory(manifest)

add_library(orchard_orchard project.cpp)
add_library(orchard::orchard ALIAS orchard_orchard)

target_sources(orchard_orchard PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES config.hpp project.hpp
)

target_link_libraries(orchard_orchard
  PUBLIC  orchard::manifest common::lexing
  PRIVATE cherry::options
)
