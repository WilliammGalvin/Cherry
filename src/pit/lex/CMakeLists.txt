add_library(pit_lex lexer.cpp)
add_library(pit::lex ALIAS pit_lex)

target_sources(pit_lex PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES lexer.hpp
)

target_link_libraries(pit_lex
  PUBLIC common::lexing
  PRIVATE cherry::options
)

add_subdirectory(debug)
