#include "cherry/semantic/semantic_analyzer.hpp"

#include <iostream>

#include "cherry/ast/stmt/assignment.hpp"
#include "cherry/ast/stmt/function_decl.hpp"
#include "cherry/semantic/semantic_error.hpp"

using namespace cherry::semantic;

SemanticAnalyzer::SemanticAnalyzer()
    : symbol_table(SymbolTable()) {

    dispatcher = {
        { typeid(ast::Program), [this](ast::ASTNode* node) {
            visit_program(dynamic_cast<ast::Program*>(node));
        }},
        { typeid(ast::FunctionDecl), [this](ast::ASTNode* node) {
            visit_function_decl(dynamic_cast<ast::FunctionDecl*>(node));
        }},
        { typeid(ast::Declaration), [this](ast::ASTNode* node) {
            visit_declaration(dynamic_cast<ast::Declaration*>(node));
        }},
        { typeid(ast::Assignment), [this](ast::ASTNode* node) {
            visit_assignment(dynamic_cast<ast::Assignment*>(node));
        }},
    };
}

void SemanticAnalyzer::visit_declaration(ast::Declaration* declaration) {
    if (symbol_table.is_declared(declaration->name)) {
        throw SemanticError("Variable '" + declaration->name + "' already declared in scope.");
    }

    symbol_table.declare(declaration->name, VARIABLE);

    // Type check value and explicit type
}

void SemanticAnalyzer::visit_assignment(ast::Assignment* assignment) {
    if (!symbol_table.is_declared(assignment->target->name)) {
        throw SemanticError("Attempting to assign variable '" + assignment->target->name + "' that isn't in scope.");
    }

    // Check if const
    // Type check value and target explicit type
}

void SemanticAnalyzer::visit_function_decl(ast::FunctionDecl* func) {
    if (symbol_table.is_declared(func->identifier->name)) {
        throw SemanticError("Function '" + func->identifier->name + "' already declared in scope.");
    }

    symbol_table.declare(func->identifier->name, FUNCTION);
    symbol_table.enter_scope();

    for (const auto& stmt : func->body) {
        analyze(stmt.get());
    }
}

void SemanticAnalyzer::visit_program(ast::Program* program) {
    for (const auto& stmt : program->stmts) {
        analyze(stmt.get());
    }
}

void SemanticAnalyzer::analyze(ast::ASTNode* node) {
    for (const auto& [type_id, handler] : dispatcher) {
        if (type_id == typeid(*node)) {
            handler(node);
            return;
        }
    }

    // throw SemanticError(std::string("Failed to find handler for node '") + typeid(*node).name() + " '.");
}
