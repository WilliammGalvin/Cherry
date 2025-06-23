cmake_minimum_required(VERSION 3.28)
project(Cherry)

set(CMAKE_CXX_STANDARD 20)

# === LLVM ===
find_package(LLVM REQUIRED CONFIG)
llvm_map_components_to_libnames(llvm_libs support core irreader)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# === Lexer Library ===
add_library(cherry_lexer
        src/lexer/lexer.cpp
        src/lexer/lex_error.cpp
        src/lexer/token.cpp
        src/lexer/token_type.cpp
)
target_include_directories(cherry_lexer PUBLIC include)

# === AST Library (Header-Only) ===
add_library(cherry_ast INTERFACE)
target_include_directories(cherry_ast INTERFACE include)

# === Parser Library ===
add_library(cherry_parser
        src/parser/parser.cpp
        src/parser/parse_error.cpp
)
target_include_directories(cherry_parser PUBLIC include)
target_link_libraries(cherry_parser PUBLIC cherry_lexer cherry_ast)

# === Semantic Library ===
add_library(cherry_semantic
        src/semantic/symbol_table.cpp
        src/semantic/semantic_analyzer.cpp
        src/semantic/semantic_error.cpp
)
target_include_directories(cherry_semantic PUBLIC include)
target_link_libraries(cherry_semantic PUBLIC cherry_ast cherry_parser)

# === Main Executable ===
add_executable(Cherry
        main.cpp
        src/ir/ir_builder.cpp

        include/cherry/semantic/semantic_error.hpp
        include/cherry/ast/types.hpp
        include/cherry/ast/expr/grouping.hpp
        include/cherry/printer/ast_printer.hpp

        include/cherry/ir/ir_values.hpp
        include/cherry/ir/ir_builder.hpp
        include/cherry/ir/ir_function_decl.hpp
        include/cherry/ir/ir_node.hpp
        include/cherry/ir/ir_program.hpp
        include/cherry/ir/ir_variable_assignment.hpp
        include/cherry/ir/ir_variable_decl.hpp
        include/cherry/printer/ir_printer.hpp
        include/cherry/llvm/llvm_code_gen.hpp
        src/llvm/llvm_code_gen.cpp
        include/cherry/printer/llvm_printer.hpp
        include/cherry/llvm/llvm_runner.hpp
        include/cherry/ir/ir_return.hpp
)

target_include_directories(Cherry PUBLIC include)
target_link_libraries(Cherry
        PRIVATE
        cherry_lexer
        cherry_parser
        cherry_ast
        cherry_semantic
        ${llvm_libs}
)
