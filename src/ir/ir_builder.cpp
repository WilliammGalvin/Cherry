#include "cherry/ir/ir_builder.hpp"

#include <algorithm>
#include <memory>

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
        return std::make_unique<IRVariableDecl>(
            decl.name,
            ast_type_to_ir_type(decl.explicit_type),
            lower_expr(*decl.initializer)
        );
    }

    std::unique_ptr<IRAssignment> IRBuilder::lower_assignment(const ast::Assignment& assignment) {
        return std::make_unique<IRAssignment>(
            assignment.target->name,
            lower_expr(*assignment.value)
        );
    }

    std::unique_ptr<IRReturn> IRBuilder::lower_return(const ast::ReturnStatement& ret) {
        return std::make_unique<IRReturn>(lower_expr(*ret.value));
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

    std::unique_ptr<IRProgram> IRBuilder::lower_program(const ast::Program& program) {
        std::vector<std::unique_ptr<IRFunction>> functions;

        for (const auto& stmt : program.stmts) {
            if (const auto* func_decl = dynamic_cast<const ast::FunctionDecl*>(stmt.get())) {
                functions.emplace_back(lower_function_decl(*func_decl));
            } else {
                throw IRError("Non-function declaration found in program body.");
            }
        }

        return std::make_unique<IRProgram>(std::move(functions));
    }

} // namespace cherry::ir
