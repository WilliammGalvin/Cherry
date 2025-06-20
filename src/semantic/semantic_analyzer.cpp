#include "cherry/semantic/semantic_analyzer.hpp"

#include "cherry/ast/stmt/assignment.hpp"
#include "cherry/ast/stmt/function_decl.hpp"

using namespace cherry::semantic;

SemanticAnalyzer::SemanticAnalyzer()
    : symbol_table(SymbolTable()) {}

void SemanticAnalyzer::analyze(ast::ASTNode* root) const {
    if (auto program = dynamic_cast<ast::Program*>(root)) {
        visit_program(program);
    } else if (auto function = dynamic_cast<ast::FunctionDecl*>(root)) {
        if (!symbol_table.declare(function->identifier->name, SymbolType::FUNCTION)) {
            throw std::runtime_error("Function already declared: " + function->identifier->name);
        }
    } else if (auto variable = dynamic_cast<ast::Assignment*>(root)) {
        if (!symbol_table.declare(variable->target->name, SymbolType::VARIABLE)) {
            throw std::runtime_error("Variable already declared: " + variable->name);
        }
    } else {
        throw std::runtime_error("Unknown AST node type for semantic analysis.");
    }
}
