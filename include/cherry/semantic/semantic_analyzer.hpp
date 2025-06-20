#ifndef SEMANTIC_ANALYZER_HPP
#define SEMANTIC_ANALYZER_HPP
#include "symbol_table.hpp"
#include "cherry/ast/ast_node.hpp"
#include "cherry/ast/program.hpp"

namespace cherry::semantic {

    class SemanticAnalyzer {
        SymbolTable symbol_table;

        void visit_program(ast::Program* program) const;

    public:
        SemanticAnalyzer();

        void analyze(ast::ASTNode* root) const;
    };

} // namespace cherry::semantic

#endif //SEMANTIC_ANALYZER_HPP
