add_subdirectory(lex)
add_subdirectory(parse)
add_subdirectory(ast)
add_subdirectory(sema)
add_subdirectory(codegen)
add_subdirectory(driver)

add_library(pit_compiler INTERFACE)
add_library(pit::compiler ALIAS pit_compiler)

target_link_libraries(pit_compiler INTERFACE
    pit::lexer
    pit::ast
    pit::parser
    pit::sema
)
