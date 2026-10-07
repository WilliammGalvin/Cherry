cherry_add_component(pit_ast
    ALIAS   pit::ast
    SOURCES ast.cpp
    HEADERS ast.hpp
    DEPENDS pit::lexer cherry::common
)
