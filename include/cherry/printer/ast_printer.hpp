#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include <ostream>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <variant>

#include <cherry/ast/program.hpp>
#include <cherry/ast/expr/binary_op.hpp>
#include <cherry/ast/expr/function_call.hpp>
#include <cherry/ast/expr/identifier.hpp>
#include <cherry/ast/expr/literal.hpp>
#include <cherry/ast/stmt/assignment.hpp>
#include <cherry/ast/stmt/declaration.hpp>
#include <cherry/ast/stmt/function_call.hpp>
#include <cherry/ast/stmt/function_decl.hpp>
#include <cherry/ast/stmt/if_statement.hpp>
#include <cherry/ast/stmt/return_statement.hpp>
#include <cherry/ast/stmt/scope_block.hpp>
#include <cherry/ast/stmt/while_loop.hpp>
#include <cherry/ast/expr/grouping.hpp>

#include "cherry/ast/expr/unary_op.hpp"

#undef DISPATCHER_ENTRY
#define DISPATCHER_ENTRY(TYPE, FUNC) \
    { typeid(TYPE), [](ast::ASTNode* node, std::ostream& os, size_t indent) { \
        FUNC(os, dynamic_cast<TYPE*>(node), indent); \
    }}

namespace cherry::ast::printer {

    inline void print_indent(std::ostream& os, size_t indent) {
        for (size_t i = 0; i < indent; ++i)
            os << "  ";
    }

    inline void print_expression(std::ostream& os, Expr* expr, size_t indent = 0);
    inline void print_stmt(std::ostream& os, Statement* stmt, size_t indent = 0);

    inline void print_unary_expr(std::ostream& os, UnaryExpr* expr, size_t indent) {
        print_indent(os, indent);
        os << "UnaryExpr (" << unary_op_to_str(expr->op) << ")\n";
        print_expression(os, expr->operand.get(), indent + 1);
    }

    inline void print_binary_expr(std::ostream& os, BinaryExpr* bin_expr, size_t indent) {
        print_indent(os, indent);
        os << "BinaryExpr (" << binary_op_to_str(bin_expr->op) << ")\n";
        print_expression(os, bin_expr->left.get(), indent + 1);
        print_expression(os, bin_expr->right.get(), indent + 1);
    }

    inline void print_function_call_expr(std::ostream& os, FunctionCallExpr* func_call, size_t indent) {
        print_indent(os, indent);
        os << "FunctionCallExpr: " << func_call->callee << "\n";
        for (const auto& arg : func_call->args) {
            print_expression(os, arg.get(), indent + 1);
        }
    }

    inline void print_grouping(std::ostream& os, GroupingExpr* grouping, size_t indent) {
        print_indent(os, indent);
        os << "GroupingExpr\n";
        print_expression(os, grouping->inner.get(), indent + 1);
    }

    inline void print_identifier(std::ostream& os, IdentifierExpr* identifier, size_t indent) {
        print_indent(os, indent);
        os << "IdentifierExpr: " << identifier->name << "\n";
    }

    inline void print_int_literal(std::ostream& os, IntegerLiteral* val, size_t indent) {
        print_indent(os, indent);
        os << "IntegerLiteral: " << std::get<long long>(val->value) << "\n";
    }

    inline void print_float_literal(std::ostream& os, FloatLiteral* val, size_t indent) {
        print_indent(os, indent);
        os << "FloatLiteral: " << std::get<double>(val->value) << "\n";
    }

    inline void print_bool_literal(std::ostream& os, BooleanLiteral* val, size_t indent) {
        print_indent(os, indent);
        os << "BooleanLiteral: " << (std::get<bool>(val->value) ? "true" : "false") << "\n";
    }

    inline void print_string_literal(std::ostream& os, StringLiteral* val, size_t indent) {
        print_indent(os, indent);
        os << "StringLiteral: \"" << std::get<std::string>(val->value) << "\"\n";
    }

    inline void print_assignment(std::ostream& os, Assignment* assignment, size_t indent) {
        print_indent(os, indent);
        os << "Assignment:\n";

        print_indent(os, indent + 1);
        if (assignment->target) {
            os << "Target: " << assignment->target->name << "\n";
        } else {
            os << "Target: <null>\n";
        }

        print_indent(os, indent + 1);
        if (assignment->value) {
            os << "Value:\n";
            print_expression(os, assignment->value.get(), indent + 2);
        } else {
            os << "Value: undefined\n";
        }
    }

    inline void print_declaration(std::ostream& os, Declaration* decl, size_t indent) {
        print_indent(os, indent);
        os << (decl->is_const ? "Const Var Declaration:\n" : "Var Declaration:\n");
        print_indent(os, indent + 1);
        os << "Name: " << decl->name << "\n";
        print_indent(os, indent + 1);
        os << "Type: " << var_type_to_str(decl->explicit_type) << "\n";

        print_indent(os, indent + 1);
        if (decl->initializer) {
            os << "Initializer:\n";
            print_expression(os, decl->initializer.get(), indent + 2);
        } else {
            os << "Initializer: null\n";
        }
    }

    inline void print_function_call_stmt(std::ostream& os, FunctionCallStatement* stmt, size_t indent) {
        print_indent(os, indent);
        os << "FunctionCallStatement: " << stmt->callee << "\n";
        for (const auto& arg : stmt->args) {
            print_expression(os, arg.get(), indent + 1);
        }
    }

    inline void print_function_decl(std::ostream& os, FunctionDecl* decl, size_t indent) {
        print_indent(os, indent);
        os << "FunctionDecl:\n";
        print_indent(os, indent + 1);
        os << "Name: " << decl->identifier->name << "\n";
        print_indent(os, indent + 1);
        os << "Params:\n";
        if (decl->params.empty()) {
            print_indent(os, indent + 2);
            os << "Empty\n";
        } else {
            for (const auto& param : decl->params) {
                print_indent(os, indent + 2);
                os << var_type_to_str(param.type) << " " << param.name << "\n";
            }
        }
        print_indent(os, indent + 1);
        os << "Body:\n";
        for (const auto& stmt : decl->body) {
            print_stmt(os, stmt.get(), indent + 2);
        }
    }

    inline void print_if_stmt(std::ostream& os, IfStatement* stmt, size_t indent) {
        print_indent(os, indent);
        os << "IfStatement\n";
        print_expression(os, stmt->condition.get(), indent + 1);
        for (const auto& s : stmt->then_statements) {
            print_stmt(os, s.get(), indent + 1);
        }
        for (const auto& s : stmt->else_statements) {
            print_stmt(os, s.get(), indent + 1);
        }
    }

    inline void print_return_stmt(std::ostream& os, ReturnStatement* stmt, size_t indent) {
        print_indent(os, indent);
        os << "ReturnStatement\n";
        if (stmt->value) print_expression(os, stmt->value.get(), indent + 1);
    }

    inline void print_scope_stmt(std::ostream& os, VisibilityScope* block, size_t indent) {
        print_indent(os, indent);

        switch (block->visibility) {
            case PUBLIC: os << "PublicScope\n"; break;
            case PRIVATE: os << "PrivateScope\n"; break;
            default: os << "UndefinedScope\n";
        }

        for (const auto& s : block->body) {
            print_stmt(os, s.get(), indent + 1);
        }
    }

    inline void print_while_stmt(std::ostream& os, WhileStatement* stmt, size_t indent) {
        print_indent(os, indent);
        os << "WhileStatement\n";
        print_expression(os, stmt->condition.get(), indent + 1);
        for (const auto& s : stmt->body) {
            print_stmt(os, s.get(), indent + 1);
        }
    }

    inline void print_for_stmt(std::ostream& os, ForStatement* stmt, size_t indent) {
        print_indent(os, indent);
        os << "ForStatement\n";

        print_indent(os, indent + 1);
        os << "Initializer\n";
        print_stmt(os, stmt->initializer.get(), indent + 2);

        print_indent(os, indent + 1);
        os << "Condition\n";
        print_expression(os, stmt->condition.get(), indent + 2);

        print_indent(os, indent + 1);
        os << "Body\n";
        for (const auto& s : stmt->body) {
            print_stmt(os, s.get(), indent + 2);
        }
    }

    static std::unordered_map<std::type_index, std::function<void(ASTNode*, std::ostream&, size_t)>> dispatcher = {
            DISPATCHER_ENTRY(ast::BinaryExpr, print_binary_expr),
            DISPATCHER_ENTRY(ast::UnaryExpr, print_unary_expr),
            DISPATCHER_ENTRY(ast::FunctionCallExpr, print_function_call_expr),
            DISPATCHER_ENTRY(ast::GroupingExpr, print_grouping),
            DISPATCHER_ENTRY(ast::IdentifierExpr, print_identifier),
            DISPATCHER_ENTRY(ast::IntegerLiteral, print_int_literal),
            DISPATCHER_ENTRY(ast::FloatLiteral, print_float_literal),
            DISPATCHER_ENTRY(ast::BooleanLiteral, print_bool_literal),
            DISPATCHER_ENTRY(ast::StringLiteral, print_string_literal),
            DISPATCHER_ENTRY(ast::Assignment, print_assignment),
            DISPATCHER_ENTRY(ast::Declaration, print_declaration),
            DISPATCHER_ENTRY(ast::FunctionCallStatement, print_function_call_stmt),
            DISPATCHER_ENTRY(ast::FunctionDecl, print_function_decl),
            DISPATCHER_ENTRY(ast::IfStatement, print_if_stmt),
            DISPATCHER_ENTRY(ast::ReturnStatement, print_return_stmt),
            DISPATCHER_ENTRY(ast::VisibilityScope, print_scope_stmt),
            DISPATCHER_ENTRY(ast::WhileStatement, print_while_stmt),
            DISPATCHER_ENTRY(ast::ForStatement, print_for_stmt)
    };

    inline void print_expression(std::ostream& os, Expr* expr, size_t indent) {
        auto it = dispatcher.find(typeid(*expr));

        if (it != dispatcher.end()) {
            it->second(expr, os, indent);
        } else {
            print_indent(os, indent);
            os << "<UnknownExpr>\n";
        }
    }

    inline void print_stmt(std::ostream& os, Statement* stmt, size_t indent) {
        auto it = dispatcher.find(typeid(*stmt));

        if (it != dispatcher.end()) {
            it->second(stmt, os, indent);
        } else {
            print_indent(os, indent);
            os << "<UnknownStmt>\n";
        }
    }

    inline void print_program(std::ostream& os, Program* program, size_t indent = 0) {
        print_indent(os, indent); os << "Program:\n";

        for (const auto& stmt : program->stmts) {
            print_stmt(os, stmt.get(), indent + 1);
        }
    }

} // namespace cherry::printer

#endif // AST_PRINTER_HPP
