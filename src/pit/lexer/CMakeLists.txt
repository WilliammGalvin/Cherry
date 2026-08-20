cherry_add_component(pit_lexer
    ALIAS   pit::lexer
    SOURCES lexer.cpp token.cpp
    HEADERS lexer.hpp token.hpp token_type.hpp
    DEPENDS cherry::common
)
