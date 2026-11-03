add_library(pit_lexer_debug INTERFACE)
add_library(pit::lexer_debug ALIAS pit_lexer_debug)

target_sources(pit_lexer_debug INTERFACE
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
      token_type.hpp
)

target_link_libraries(pit_lexer_debug INTERFACE
  pit::lexer
  common::lexing_debug
)
