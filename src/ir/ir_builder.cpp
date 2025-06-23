#include "cherry/ir/ir_builder.hpp"

#include <cherry/ast/expr/literal.hpp>

using namespace cherry::ir;

IRReturn* IRBuilder::lower_return(ast::ReturnStatement* return_stmt) {
    IRValue* value = nullptr;

    if (auto int_literal = dynamic_cast<ast::IntegerLiteral*>(return_stmt->value.get())) {
        value = new IRConstant(ast::INT, std::to_string(int_literal->value));
    }

    return new IRReturn(value);
}

IRFunctionDecl* IRBuilder::lower_function_decl(ast::FunctionDecl* func_decl) {
    std::vector<IRParam*> params;
    params.reserve(func_decl->params.size());

    for (const auto& ast_param : func_decl->params) {
        params.emplace_back(new IRParam(ast_param.name, ast_param.type));
    }

    std::vector<IRNode*> body;

    for (const auto& stmt : func_decl->body) {
        if (auto var_decl = dynamic_cast<ast::Declaration*>(stmt.get())) {
            const auto pair = lower_variable_decl(var_decl);
            body.emplace_back(pair.first);
            body.emplace_back(pair.second);
        } else if (auto assignment = dynamic_cast<ast::Assignment*>(stmt.get())) {
            body.emplace_back(lower_variable_assignment(assignment));
        }
    }

    return new IRFunctionDecl(
        func_decl->identifier->name,
        func_decl->return_type,
        std::move(params),
        std::move(body)
    );
}

std::pair<IRVariableDecl*, IRVariableAssignment*> IRBuilder::lower_variable_decl(ast::Declaration* decl) {
    IRValue* value = nullptr;

    if (auto int_literal = dynamic_cast<ast::IntegerLiteral*>(decl->initializer.get())) {
        value = new IRConstant(ast::INT, std::to_string(int_literal->value));
    }

    auto var_decl = new IRVariableDecl(decl->name, decl->explicit_type);
    auto var_assignment = new IRVariableAssignment(decl->name, value);

    return { var_decl, var_assignment };
}

IRVariableAssignment* IRBuilder::lower_variable_assignment(ast::Assignment* assignment) {
    IRValue* value = nullptr;

    if (auto int_literal = dynamic_cast<ast::IntegerLiteral*>(assignment->value.get())) {
        value = new IRConstant(ast::INT, std::to_string(int_literal->value));
    }

    return new IRVariableAssignment(assignment->target->name, value);
}

IRProgram* IRBuilder::lower_program(ast::Program* program) {
    std::vector<IRNode*> nodes;

    for (const auto& stmt : program->stmts) {
        if (const auto func = dynamic_cast<ast::FunctionDecl*>(stmt.get())) {
            nodes.emplace_back(lower_function_decl(func));
        }
    }

    return new IRProgram(std::move(nodes));
}
