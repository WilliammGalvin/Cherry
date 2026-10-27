add_library(orchard_manifest
  lexer.cpp
  parser.cpp
)

add_library(orchard::manifest ALIAS orchard_manifest)

target_sources(orchard_manifest PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
      manifest.hpp
      lexer.hpp
      parser.hpp
      token_type.hpp
)

target_link_libraries(orchard_manifest
  PUBLIC  common::lexing
  PRIVATE cherry::options
)

add_subdirectory(debug)
