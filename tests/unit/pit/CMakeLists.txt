cherry_add_test(pit_lexer_test
    SOURCES lexer_test.cpp
    DEPENDS pit::lexer cherry::common
)

cherry_add_test(pit_ast_test
    SOURCES ast_test.cpp
    DEPENDS pit::ast pit::lexer cherry::common
)

cherry_add_test(pit_parser_test
    SOURCES parser_test.cpp
    DEPENDS pit::parser pit::ast pit::lexer
)
