cherry_add_component(pit_sema
    ALIAS   pit::sema
    SOURCES sema.cpp
    HEADERS sema.hpp
    DEPENDS pit::ast pit::lexer
)
