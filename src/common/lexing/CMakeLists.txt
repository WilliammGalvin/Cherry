add_library(common_lexing source_manager.cpp)
add_library(common::lexing ALIAS common_lexing)

target_sources(common_lexing PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
    lex_error.hpp
    source_cursor.hpp
    source_file.hpp
    source_manager.hpp
    token.hpp
)

target_link_libraries(common_lexing PRIVATE cherry::options)

add_subdirectory(debug)
