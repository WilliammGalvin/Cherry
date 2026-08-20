#   cherry_add_component(pit_lexer
#       ALIAS   pit::lexer
#       SOURCES lexer.cpp token.cpp
#       HEADERS lexer.hpp token.hpp token_type.hpp
#       DEPENDS cherry::common)

function(cherry_add_component name)
  if(NOT CHERRY_INCLUDE_ROOT)
    message(FATAL_ERROR "CHERRY_INCLUDE_ROOT is not set")
  endif()

  cmake_parse_arguments(ARG "" "ALIAS" "SOURCES;HEADERS;DEPENDS" ${ARGN})

  add_library(${name} STATIC ${ARG_SOURCES})

  if(ARG_ALIAS)
    add_library(${ARG_ALIAS} ALIAS ${name})
  endif()

  if(ARG_HEADERS)
    target_sources(${name}
            PUBLIC
                FILE_SET HEADERS
                BASE_DIRS ${CHERRY_INCLUDE_ROOT}
                FILES ${ARG_HEADERS}
        )
  endif()

  target_include_directories(${name} PUBLIC ${CHERRY_INCLUDE_ROOT})
  target_link_libraries(${name}
        PUBLIC ${ARG_DEPENDS}
        PRIVATE cherry::warnings
    )
endfunction()
