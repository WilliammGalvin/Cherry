cherry_add_component(pit_sema
    ALIAS   pit::sema
    SOURCES context.cpp declare.cpp intern_table.cpp resolve.cpp type.cpp
    HEADERS arena.hpp context.hpp decl_gather.hpp
            declare.hpp def.hpp diagnostic.hpp file_id.hpp
            intern_table.hpp node_layout.hpp resolve.hpp
            scope.hpp type.hpp
    DEPENDS pit::ast pit::lexer
)
