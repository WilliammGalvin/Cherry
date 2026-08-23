cherry_add_component(pit_ast
    ALIAS   pit::ast
    SOURCES ast.cpp printer.cpp
    HEADERS ast.hpp printer.hpp
    DEPENDS pit::lexer cherry::common
)
