add_library(common_lexing_debug INTERFACE)
add_library(common::lexing_debug ALIAS common_lexing_debug)

target_sources(common_lexing_debug INTERFACE
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
    lex_error.hpp
    token.hpp
)

target_link_libraries(common_lexing_debug INTERFACE common::lexing)
