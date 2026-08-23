cherry_add_component(pit_parser
    ALIAS   pit::parser
    SOURCES parser.cpp
    HEADERS parser.hpp
    DEPENDS pit::ast pit::lexer
)
