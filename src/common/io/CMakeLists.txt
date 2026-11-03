add_library(common_io source_manager.cpp)
add_library(common::io ALIAS common_io)

target_sources(common_io PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
    read_source_file.hpp
    source_manager.hpp
)

target_link_libraries(common_io PRIVATE cherry::options)

add_subdirectory(debug)
