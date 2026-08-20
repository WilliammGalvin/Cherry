add_subdirectory(lexer)

add_library(pit_compiler INTERFACE)
add_library(pit::compiler ALIAS pit_compiler)

target_link_libraries(pit_compiler INTERFACE
    pit::lexer
)
