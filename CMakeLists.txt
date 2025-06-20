cmake_minimum_required(VERSION 3.30)
project(Cherry)

set(CMAKE_CXX_STANDARD 20)

# === LLVM ===
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

llvm_map_components_to_libnames(llvm_libs support core irreader)

# === Lexer Library ===
add_library(cherry_lexer
        src/lexer/lexer.cpp
        src/lexer/lex_error.cpp
        src/lexer/token.cpp
        src/lexer/token_type.cpp
)

# Public headers are in include/
target_include_directories(cherry_lexer PUBLIC include)

# === Main Executable ===
add_executable(Cherry main.cpp
        include/cherry/parser/parser.hpp
        include/cherry/parser/parse_error.hpp
        include/cherry/ast/ast_node.hpp
        include/cherry/ast/program.hpp
        include/cherry/ast/stmt/statement.hpp
        include/cherry/ast/expr/expression.hpp
        include/cherry/ast/stmt/declaration.hpp
        include/cherry/ast/stmt/assignment.hpp
        include/cherry/ast/stmt/if_statement.hpp
        include/cherry/ast/stmt/while_loop.hpp
        include/cherry/ast/stmt/function_call.hpp
        include/cherry/ast/stmt/function_decl.hpp
        include/cherry/ast/stmt/return_statement.hpp
        include/cherry/ast/expr/literal.hpp
        include/cherry/ast/expr/identifier.hpp
        include/cherry/ast/expr/binary_op.hpp
        include/cherry/ast/expr/unary_op.hpp
        include/cherry/ast/expr/grouping.hpp
        src/parser/parse_error.cpp
        src/parser/parser.cpp
        include/cherry/ast/expr/function_call.hpp
        include/cherry/ast/stmt/scope_blocks.hpp
        src/ast/ast_node.cpp
        include/cherry/semantic/symbol_table.hpp
        include/cherry/semantic/semantic_analyzer.hpp
        src/semantic/symbol_table.cpp
        src/semantic/semantic_analyzer.cpp
)

# Link lexer library + LLVM to Cherry
target_link_libraries(Cherry PRIVATE cherry_lexer ${llvm_libs})
