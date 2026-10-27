cherry_add_component(pit_parser
    ALIAS   pit::parser
    SOURCES parser.cpp
    HEADERS parser.hpp parse_error.hpp source_reader.hpp token_cursor.hpp
    DEPENDS pit::ast pit::lexer
)
