add_library(common_lexing INTERFACE)
add_library(common::lexing ALIAS common_lexing)

target_sources(common_lexing INTERFACE
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
    lex_error.hpp
    token.hpp
)

add_subdirectory(debug)
