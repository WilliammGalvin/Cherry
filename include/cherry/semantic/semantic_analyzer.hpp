#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP
#include <functional>
#include <typeindex>
#include <cherry/ast/stmt/assignment.hpp>
#include <cherry/ast/stmt/declaration.hpp>
#include <cherry/ast/stmt/function_decl.hpp>

#include "symbol_table.hpp"
#include "cherry/ast/ast_node.hpp"
#include "cherry/ast/program.hpp"

namespace cherry::semantic {

    class SemanticAnalyzer {
        SymbolTable symbol_table;

        std::unordered_map<std::type_index, std::function<void(ast::ASTNode*)>> dispatcher;

        void visit_declaration(ast::Declaration* declaration);
        void visit_assignment(ast::Assignment* assignment);
        void visit_function_decl(ast::FunctionDecl* func);
        void visit_program(ast::Program* program);

    public:
        SemanticAnalyzer();

        void analyze(ast::ASTNode* node);
    };

} // namespace cherry::semantic

#endif //SEMANTIC_ANALYZER_HPP
