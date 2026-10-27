add_library(pit_lexer lexer.cpp)
add_library(pit::lexer ALIAS pit_lexer)

target_sources(pit_lexer PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES lexer.hpp char_class.hpp
)

target_link_libraries(pit_lexer
  PUBLIC common::lexing
  PRIVATE cherry::options
)

add_subdirectory(debug)
