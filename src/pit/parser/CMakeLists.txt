add_library(pit_parser parser.cpp)
add_library(pit::parser ALIAS pit_parser)

target_sources(pit_parser PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES parse_error.hpp parser.hpp token_cursor.hpp
)

target_link_libraries(pit_parser
  PUBLIC pit::lexer pit::ast common::lexing
  PRIVATE cherry::options
)

add_subdirectory(debug)
