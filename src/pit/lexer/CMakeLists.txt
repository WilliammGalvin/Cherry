cherry_add_component(pit_lexer
    ALIAS   pit::lexer
    SOURCES lexer.cpp
    HEADERS lexer.hpp token.hpp token_type.hpp
            lex_error.hpp source_cursor.hpp char_class.hpp
    DEPENDS cherry::common
)
