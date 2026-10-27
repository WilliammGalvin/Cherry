add_library(pit_sema
  context.cpp
  declare.cpp
  intern_table.cpp
  resolve.cpp
  type.cpp
)

add_library(pit::sema ALIAS pit_sema)

target_sources(pit_sema PUBLIC
  FILE_SET HEADERS
  BASE_DIRS ${PROJECT_SOURCE_DIR}/src
  FILES
      arena.hpp
      context.hpp
      decl_gather.hpp
      declare.hpp
      def.hpp
      diagnostic.hpp
      file_id.hpp
      intern_table.hpp
      name.hpp
      node_layout.hpp
      resolve.hpp
      scope.hpp
      type.hpp
)

target_link_libraries(pit_sema
  PUBLIC  pit::ast common::lexing
  PRIVATE cherry::options
)

add_subdirectory(debug)
