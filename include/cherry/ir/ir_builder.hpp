#ifndef IR_BUILDER_HPP
#define IR_BUILDER_HPP

#include <vector>
#include <memory>

#include "ir_function.hpp"
#include "ir_program.hpp"
#include "cherry/ast/program.hpp"
#include "cherry/ast/expr/binary_op.hpp"
#include "cherry/ast/expr/expression.hpp"
#include "cherry/ast/expr/function_call.hpp"
#include "cherry/ast/expr/identifier.hpp"
#include "cherry/ast/expr/literal.hpp"
#include "cherry/ast/expr/unary_op.hpp"
#include "cherry/ast/stmt/assignment.hpp"
#include "cherry/ast/stmt/declaration.hpp"
#include "cherry/ast/stmt/function_decl.hpp"
#include "cherry/ast/stmt/return_statement.hpp"
#include "instructions/ir_assignment.hpp"
#include "instructions/ir_return.hpp"
#include "instructions/ir_variable_decl.hpp"
#include "values/ir_binary.hpp"
#include "values/ir_constant.hpp"
#include "values/ir_function_call.hpp"
#include "values/ir_identifier.hpp"
#include "values/ir_unary.hpp"

namespace cherry::ir {

    class IRBuilder {
        std::unique_ptr<IRValue> lower_expr(const ast::Expr& expr);
        std::unique_ptr<IRInstruction> lower_stmt(const ast::Statement& stmt);
        std::vector<std::unique_ptr<IRInstruction>> lower_body(const std::vector<std::unique_ptr<ast::Statement>>& stmts);

        std::unique_ptr<IRIdentifier> lower_identifier(const ast::IdentifierExpr& id);
        std::unique_ptr<IRConstant> lower_constant(const ast::Literal& literal);
        std::unique_ptr<IRUnary> lower_unary(const ast::UnaryExpr& unary);
        std::unique_ptr<IRBinary> lower_binary(const ast::BinaryExpr& binary);
        std::unique_ptr<IRFunctionCall> lower_function_call(const ast::FunctionCallExpr& call);

        std::unique_ptr<IRVariableDecl> lower_declaration(const ast::Declaration& decl);
        std::unique_ptr<IRAssignment> lower_assignment(const ast::Assignment& assignment);
        std::unique_ptr<IRReturn> lower_return(const ast::ReturnStatement& ret);

        std::unique_ptr<IRFunction> lower_function_decl(const ast::FunctionDecl& func);

    public:
        std::unique_ptr<IRProgram> lower_program(const ast::Program& program);
    };

} // namespace cherry::ir

#endif // IR_BUILDER_HPP
