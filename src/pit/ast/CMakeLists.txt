cherry_add_component(pit_ast
    ALIAS   pit::ast
    HEADERS node.hpp tree.hpp extra.hpp node_tag.hpp
    DEPENDS pit::lexer cherry::common
)
