#include "cherry/ir/ir_builder.hpp"

#include <algorithm>
#include <memory>

#include "cherry/ast/stmt/for_statement.hpp"
#include "cherry/ast/stmt/scope_block.hpp"
#include "cherry/ir/ir_error.hpp"

namespace cherry::ir {

    std::unique_ptr<IRValue> IRBuilder::lower_expr(const ast::Expr& expr) {
        if (auto* id = dynamic_cast<const ast::IdentifierExpr*>(&expr)) {
            return lower_identifier(*id);
        }
        if (auto* literal = dynamic_cast<const ast::Literal*>(&expr)) {
            return lower_constant(*literal);
        }
        if (auto* unary = dynamic_cast<const ast::UnaryExpr*>(&expr)) {
            return lower_unary(*unary);
        }
        if (auto* binary = dynamic_cast<const ast::BinaryExpr*>(&expr)) {
            return lower_binary(*binary);
        }
        if (auto* call = dynamic_cast<const ast::FunctionCallExpr*>(&expr)) {
            return lower_function_call(*call);
        }

        throw IRError("Unsupported expression type in IRBuilder.");
    }

    std::unique_ptr<IRInstruction> IRBuilder::lower_stmt(const ast::Statement& stmt) {
        if (auto* decl = dynamic_cast<const ast::Declaration*>(&stmt)) {
            return lower_declaration(*decl);
        }

        if (auto* assign = dynamic_cast<const ast::Assignment*>(&stmt)) {
            return lower_assignment(*assign);
        }

        if (auto* ret = dynamic_cast<const ast::ReturnStatement*>(&stmt)) {
            return lower_return(*ret);
        }

        if (auto* cont = dynamic_cast<const ast::ContinueStatement*>(&stmt)) {
            return lower_continue(*cont);
        }

        if (auto* brk = dynamic_cast<const ast::BreakStatement*>(&stmt)) {
            return lower_break(*brk);
        }

        if (auto* if_stmt = dynamic_cast<const ast::IfStatement*>(&stmt)) {
            return lower_if_stmt(*if_stmt);
        }

        if (auto* while_stmt = dynamic_cast<const ast::WhileStatement*>(&stmt)) {
            return lower_while_stmt(*while_stmt);
        }

        if (auto* for_stmt = dynamic_cast<const ast::ForStatement*>(&stmt)) {
            return lower_for_stmt(*for_stmt);
        }

        if (auto* dir = dynamic_cast<const ast::SysCallDirective*>(&stmt)) {
            return lower_sys_call_directive(*dir);
        }

        if (auto* local_scope = dynamic_cast<const ast::VisibilityScope*>(&stmt)) {
            return lower_local_scope(*local_scope);
        }

        throw IRError("Unsupported statement type in IRBuilder.");
    }

    std::vector<std::unique_ptr<IRInstruction>> IRBuilder::lower_body(
        const std::vector<std::unique_ptr<ast::Statement>>& stmts
    ) {
        std::vector<std::unique_ptr<IRInstruction>> body;
        body.reserve(stmts.size());

        for (const auto& stmt : stmts) {
            body.emplace_back(lower_stmt(*stmt));
        }

        return body;
    }

    std::unique_ptr<IRIdentifier> IRBuilder::lower_identifier(const ast::IdentifierExpr& id) {
        return std::make_unique<IRIdentifier>(id.name);
    }

    std::unique_ptr<IRConstant> IRBuilder::lower_constant(const ast::Literal& literal) {
        switch (literal.type) {
            case ast::STRING:
                return std::make_unique<IRConstant>(
                    std::get<std::string>(literal.value),
                    ast_type_to_ir_type(literal.type)
                );
            case ast::INT:
                return std::make_unique<IRConstant>(
                    std::get<long long>(literal.value),
                    ast_type_to_ir_type(literal.type)
                );
            case ast::FLOAT:
                return std::make_unique<IRConstant>(
                    std::get<double>(literal.value),
                    ast_type_to_ir_type(literal.type)
                );
            case ast::BOOL:
                return std::make_unique<IRConstant>(
                    std::get<bool>(literal.value),
                    ast_type_to_ir_type(literal.type)
                );
            default:
                throw IRError("Unsupported literal type.");
        }
    }

    std::unique_ptr<IRUnary> IRBuilder::lower_unary(const ast::UnaryExpr& unary) {
        return std::make_unique<IRUnary>(
            unary_op_ast_to_ir(unary.op),
            lower_expr(*unary.operand)
        );
    }

    std::unique_ptr<IRBinary> IRBuilder::lower_binary(const ast::BinaryExpr& binary) {
        return std::make_unique<IRBinary>(
            binary_op_ast_to_ir(binary.op),
            lower_expr(*binary.left),
            lower_expr(*binary.right)
        );
    }

    std::unique_ptr<IRFunctionCall> IRBuilder::lower_function_call(const ast::FunctionCallExpr& call) {
        std::vector<std::unique_ptr<IRValue>> args;
        args.reserve(call.args.size());

        for (const auto& arg : call.args) {
            args.emplace_back(lower_expr(*arg));
        }

        return std::make_unique<IRFunctionCall>(call.callee, std::move(args));
    }

    std::unique_ptr<IRVariableDecl> IRBuilder::lower_declaration(const ast::Declaration& decl) {
        std::unique_ptr<IRValue> init = nullptr;

        if (decl.initializer) {
            init = lower_expr(*decl.initializer);
        }

        return std::make_unique<IRVariableDecl>(
            decl.name,
            ast_type_to_ir_type(decl.explicit_type),
            std::move(init)
        );
    }

    std::unique_ptr<IRAssignment> IRBuilder::lower_assignment(const ast::Assignment& assignment) {
        return std::make_unique<IRAssignment>(
            assignment.target->name,
            lower_expr(*assignment.value)
        );
    }

    std::unique_ptr<IRReturn> IRBuilder::lower_return(const ast::ReturnStatement& ret) {
        std::unique_ptr<IRValue> value = nullptr;

        if (ret.value) {
            value = lower_expr(*ret.value);
        }

        return std::make_unique<IRReturn>(std::move(value));
    }

    std::unique_ptr<IRContinue> IRBuilder::lower_continue(const ast::ContinueStatement& cont) {
        return std::make_unique<IRContinue>();
    }

    std::unique_ptr<IRBreak> IRBuilder::lower_break(const ast::BreakStatement& brk) {
        return std::make_unique<IRBreak>();
    }

    std::unique_ptr<IRIf> IRBuilder::lower_if_stmt(const ast::IfStatement& if_stmt) {
        auto condition = lower_expr(*if_stmt.condition);
        auto then_body = lower_body(if_stmt.then_statements);
        std::vector<std::unique_ptr<IRInstruction>> else_body;

        if (!if_stmt.else_statements.empty()) {
            else_body = lower_body(if_stmt.else_statements);
        }

        return std::make_unique<IRIf>(
            std::move(condition),
            std::move(then_body),
            std::move(else_body)
        );
    }

    std::unique_ptr<IRWhile> IRBuilder::lower_while_stmt(const ast::WhileStatement& while_stmt) {
        auto condition = lower_expr(*while_stmt.condition);
        auto body = lower_body(while_stmt.body);

        return std::make_unique<IRWhile>(
            std::move(condition),
            std::move(body)
        );
    }

    std::unique_ptr<IRFor> IRBuilder::lower_for_stmt(const ast::ForStatement& for_stmt) {
        std::unique_ptr<IRInstruction> init = nullptr;
        if (for_stmt.initializer) {
            init = lower_stmt(*for_stmt.initializer);
        }

        auto condition = lower_expr(*for_stmt.condition);
        std::unique_ptr<IRAssignment> increment = nullptr;

        if (for_stmt.increment) {
            increment = lower_assignment(*for_stmt.increment);
        }

        auto body = lower_body(for_stmt.body);

        return std::make_unique<IRFor>(
            std::move(init),
            std::move(condition),
            std::move(increment),
            std::move(body)
        );
    }

    std::unique_ptr<IRSysCallDirective> IRBuilder::lower_sys_call_directive(const ast::SysCallDirective& sys_call) {
        return std::make_unique<IRSysCallDirective>(sys_call.call_name, sys_call.content);
    }

    std::unique_ptr<IRFunction> IRBuilder::lower_function_decl(const ast::FunctionDecl& func) {
        std::vector<IRParam> params;
        params.reserve(func.params.size());

        for (const auto& param : func.params) {
            params.emplace_back(param.name, ast_type_to_ir_type(param.type));
        }

        auto body = lower_body(func.body);

        return std::make_unique<IRFunction>(
            func.identifier->name,
            std::move(params),
            ast_type_to_ir_type(func.return_type),
            std::move(body)
        );
    }

    std::unique_ptr<IRLocalScope> IRBuilder::lower_local_scope(const ast::VisibilityScope& scope) {
        if (scope.visibility != ast::LOCAL) {
            throw IRError("Unknown local scope type.");
        }

        auto body = lower_body(scope.body);
        return std::make_unique<IRLocalScope>(LOCAL, std::move(body));
    }

    std::unique_ptr<IRGlobalScope> IRBuilder::lower_global_scope(const ast::VisibilityScope& scope) {
        std::vector<std::unique_ptr<IRFunction>> functions;
        ScopeVisibility visibility;

        switch (scope.visibility) {
            case ast::PUBLIC: visibility = PUBLIC; break;
            case ast::PRIVATE: visibility = PRIVATE; break;
            default: throw IRError("Unknown global scope type.");
        }

        for (const auto& stmt : scope.body) {
            if (const auto func = dynamic_cast<ast::FunctionDecl*>(stmt.get())) {
                functions.emplace_back(lower_function_decl(*func));
                continue;
            }

            throw IRError("Expected function declaration in global scope body.");
        }

        return std::make_unique<IRGlobalScope>(visibility, std::move(functions));
    }

    std::unique_ptr<IRProgram> IRBuilder::lower_program(const ast::Program& program) {
        std::vector<std::unique_ptr<IRGlobalScope>> scopes;

        for (const auto& stmt : program.stmts) {
            if (const auto* scope = dynamic_cast<const ast::VisibilityScope*>(stmt.get())) {
                scopes.emplace_back(lower_global_scope(*scope));
            } else {
                throw IRError("Non-scope declaration found in program body.");
            }
        }

        return std::make_unique<IRProgram>(std::move(scopes));
    }

} // namespace cherry::ir
