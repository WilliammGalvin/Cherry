add_subdirectory(lexer)
add_subdirectory(ast)
add_subdirectory(parser)

add_library(pit_compiler INTERFACE)
add_library(pit::compiler ALIAS pit_compiler)

target_link_libraries(pit_compiler INTERFACE
    pit::lexer
    pit::ast
    pit::parser
)
