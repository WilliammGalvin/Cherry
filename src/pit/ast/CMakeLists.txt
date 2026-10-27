add_library(pit_ast ast.cpp)
add_library(pit::ast ALIAS pit_ast)

target_sources(pit_ast PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES extra.hpp node.hpp node_tag.hpp tree.hpp
)

target_link_libraries(pit_ast
  PUBLIC common::lexing
  PRIVATE cherry::options
)

add_subdirectory(debug)
