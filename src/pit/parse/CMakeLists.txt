add_library(pit_parse parser.cpp)
add_library(pit::parse ALIAS pit_parse)

target_sources(pit_parse PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES parse_error.hpp parser.hpp token_cursor.hpp
)

target_link_libraries(pit_parse
  PUBLIC pit::lex pit::ast common::lexing
  PRIVATE cherry::options
)

add_subdirectory(debug)
