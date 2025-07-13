cmake_minimum_required(VERSION 3.28)
project(Cherry)

set(CMAKE_CXX_STANDARD 20)

# === Include Path ===
include_directories(include)

# === Lexer Library ===
add_library(cherry_lexer
        src/lexer/lexer.cpp
        src/lexer/lex_error.cpp
        src/lexer/token.cpp
        src/lexer/token_type.cpp
)

# === AST Library (Header-only) ===
add_library(cherry_ast INTERFACE)
target_include_directories(cherry_ast INTERFACE include)

# === Parser Library ===
add_library(cherry_parser
        src/parser/parser.cpp
        src/parser/parse_error.cpp
)
target_link_libraries(cherry_parser PUBLIC cherry_lexer cherry_ast)

# === Semantic Analysis Library ===
add_library(cherry_semantic
        src/semantic/symbol_table.cpp
        src/semantic/semantic_analyzer.cpp
        src/semantic/semantic_error.cpp
)
target_link_libraries(cherry_semantic PUBLIC cherry_ast cherry_parser)

# === Main Executable ===
add_executable(Cherry
        main.cpp

        # IR Sources
        src/ir/ir_builder.cpp
        src/ir/ir_error.cpp

        # IR Includes
        include/cherry/ir/ir_builder.hpp
        include/cherry/ir/ir_function.hpp
        include/cherry/ir/ir_program.hpp
        include/cherry/ir/ir_param.hpp
        include/cherry/ir/ir_type.hpp
        include/cherry/ir/ir_error.hpp

        include/cherry/ir/instructions/ir_instruction.hpp
        include/cherry/ir/instructions/ir_assignment.hpp
        include/cherry/ir/instructions/ir_variable_decl.hpp
        include/cherry/ir/instructions/ir_return.hpp

        include/cherry/ir/values/ir_value.hpp
        include/cherry/ir/values/ir_constant.hpp
        include/cherry/ir/values/ir_identifier.hpp
        include/cherry/ir/values/ir_binary.hpp
        include/cherry/ir/values/ir_unary.hpp
        include/cherry/ir/values/ir_function_call.hpp

        # Printers
        include/cherry/debug/ast_printer.hpp
        include/cherry/debug/ir_printer.hpp

        # Other headers
        include/cherry/semantic/semantic_error.hpp
        include/cherry/ast/types.hpp
        include/cherry/ast/expr/grouping.hpp
        include/cherry/ir/instructions/ir_if.hpp
        include/cherry/ir/instructions/ir_while.hpp
        include/cherry/ast/stmt/for_statement.hpp
        include/cherry/ir/ir_scope.hpp
        include/cherry/ast/stmt/continue_statement.hpp
        include/cherry/ast/stmt/break_statement.hpp
        include/cherry/ir/instructions/ir_continue.hpp
        include/cherry/ir/instructions/ir_break.hpp
        include/cherry/ir/instructions/ir_for.hpp
        include/cherry/ast/stmt/directive_statement.hpp
        include/cherry/ir/instructions/ir_directives.hpp
)

target_link_libraries(Cherry
        PRIVATE
        cherry_lexer
        cherry_parser
        cherry_ast
        cherry_semantic
)
