#ifndef IR_PRINTER_HPP
#define IR_PRINTER_HPP

#include <ostream>
#include <memory>
#include <variant>

#include "cherry/ir/ir_program.hpp"
#include "cherry/ir/ir_function.hpp"
#include "cherry/ir/instructions/ir_assignment.hpp"
#include "cherry/ir/instructions/ir_return.hpp"
#include "cherry/ir/instructions/ir_variable_decl.hpp"
#include "cherry/ir/values/ir_constant.hpp"
#include "cherry/ir/values/ir_identifier.hpp"
#include "cherry/ir/values/ir_binary.hpp"
#include "cherry/ir/values/ir_unary.hpp"
#include "cherry/ir/values/ir_function_call.hpp"
#include "cherry/ir/instructions/ir_if.hpp"
#include "cherry/ir/instructions/ir_while.hpp"

namespace cherry::ir::printer {

    inline void print_indent(std::ostream& os, size_t indent) {
        for (size_t i = 0; i < indent; ++i) os << "  ";
    }

    inline void print_value(
        std::ostream& os,
        const std::unique_ptr<IRValue>& value,
        size_t indent = 0
    );

    inline void print_node(
        std::ostream& os,
        IRInstruction* node,
        size_t indent = 0
    );

    inline void print_function_decl(
    std::ostream& os,
    IRFunction* func,
    size_t indent = 0
) {
        print_indent(os, indent);
        os << "FunctionDecl:" << '\n';

        print_indent(os, indent + 1);
        os << "Name: " << func->name << '\n';

        print_indent(os, indent + 1);
        os << "ReturnType: " << ir_type_to_str(func->return_type) << '\n';

        if (func->params.empty()) {
            print_indent(os, indent+1);
            os << "Params: Empty\n";
        } else {
            print_indent(os, indent + 1);
            os << "Params:" << '\n';
            for (const auto& param : func->params) {
                print_indent(os, indent + 2);
                os << param.name
                   << " : "
                   << ir_type_to_str(param.type)
                   << '\n';
            }
        }

        if (func->body.empty()) {
            print_indent(os, indent + 1);
            os << "Body: Empty\n";
        } else {
            print_indent(os, indent + 1);
            os << "Body:" << '\n';
            for (const auto& stmt : func->body) {
                print_node(os, stmt.get(), indent + 2);
            }
        }
    }

    inline void print_scope(
        std::ostream& os,
        IRScope* scope,
        size_t indent = 0
    ) {
        print_indent(os, indent);

        switch (scope->visibility) {
            case PUBLIC: os << "PublicScope:" << '\n'; break;
            case PRIVATE: os << "PrivateScope:" << '\n'; break;
            default: os << "UnknownScope:" << '\n';
        }

        for (const auto& func : scope->functions) {
            print_function_decl(os, func.get(), indent + 1);
        }
    }

    inline void print_program(
        std::ostream& os,
        IRProgram* program,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "Program:" << '\n';
        for (const auto& scope : program->scopes) {
            print_scope(os, scope.get(), indent + 1);
        }
    }

    inline void print_variable_decl(std::ostream& os, IRVariableDecl* decl, size_t indent) {
        print_indent(os, indent);
        os << "VariableDecl:\n";

        print_indent(os, indent+1);
        os << "Name: %" << decl->name << "\n";

        print_indent(os, indent+1);
        os << "Type: " << ir_type_to_str(decl->type) << "\n";

        if (decl->initializer) {
            print_indent(os, indent+1);
            os << "Initializer:\n";
            print_value(os, decl->initializer, indent+2);
            os << "\n";
        }
    }

    inline void print_assignment(
        std::ostream& os,
        IRAssignment* assign,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "Assignment:" << '\n';

        print_indent(os, indent + 1);
        os << "Target: %" << assign->target << '\n';

        print_indent(os, indent + 1);
        os << "Value:" << '\n';
        print_value(os, assign->value, indent + 2);
        os << '\n';
    }

    inline void print_return(
        std::ostream& os,
        IRReturn* ret,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "Return:" << '\n';

        print_indent(os, indent + 1);
        os << "Value:" << '\n';
        print_value(os, ret->value, indent + 2);
        os << '\n';
    }

    inline void print_if(
    std::ostream& os,
    IRIf* ifstmt,
    size_t indent
) {
        print_indent(os, indent);
        os << "If:\n";

        print_indent(os, indent + 1);
        os << "Condition:\n";
        print_value(os, ifstmt->condition, indent + 2);

        print_indent(os, indent + 1);
        os << "Then:\n";
        for (const auto& stmt : ifstmt->then_body) {
            print_node(os, stmt.get(), indent + 2);
        }

        print_indent(os, indent + 1);
        os << "Else:\n";
        for (const auto& stmt : ifstmt->else_body) {
            print_node(os, stmt.get(), indent + 2);
        }
    }

    inline void print_while(
        std::ostream& os,
        IRWhile* while_stmt,
        size_t indent
    ) {
        print_indent(os, indent);
        os << "While:\n";

        print_indent(os, indent + 1);
        os << "Condition:\n";
        print_value(os, while_stmt->condition, indent + 2);

        print_indent(os, indent + 1);
        os << "Body:\n";
        for (const auto& stmt : while_stmt->body) {
            print_node(os, stmt.get(), indent + 2);
        }
    }

    inline void print_function_call(
        std::ostream& os,
        IRFunctionCall* call,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "FunctionCall: " << call->callee << '\n';

        print_indent(os, indent + 1);
        os << "Args:" << '\n';
        for (const auto& arg : call->arguments) {
            print_value(os, arg, indent + 2);
            os << '\n';
        }
    }

    inline void print_binary(
        std::ostream& os,
        IRBinary* bin,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "BinaryExpr (" << binary_op_to_str(bin->op) << ")" << '\n';

        print_value(os, bin->left, indent + 1);
        os << '\n';
        print_value(os, bin->right, indent + 1);
        os << '\n';
    }

    inline void print_unary(
        std::ostream& os,
        IRUnary* unary,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "UnaryExpr (" << unary_op_to_str(unary->op) << ")" << '\n';

        print_value(os, unary->operand, indent + 1);
        os << '\n';
    }

    inline void print_constant(
        std::ostream& os,
        IRConstant* c,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "Constant: ";
        std::visit([&](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                os << '"' << val << '"';
            } else if constexpr (std::is_same_v<T, bool>) {
                os << (val ? "true" : "false");
            } else {
                os << val;
            }
        }, c->value);
    }

    inline void print_identifier(
        std::ostream& os,
        IRIdentifier* id,
        size_t indent = 0
    ) {
        print_indent(os, indent);
        os << "Identifier: " << id->name;
    }

    inline void print_value(
        std::ostream& os,
        const std::unique_ptr<IRValue>& value,
        size_t indent
    ) {
        if (!value) {
            print_indent(os, indent);
            os << "<null>";
            return;
        }
        if (auto* c = dynamic_cast<IRConstant*>(value.get())) {
            print_constant(os, c, indent);
        } else if (auto* id = dynamic_cast<IRIdentifier*>(value.get())) {
            print_identifier(os, id, indent);
        } else if (auto* bin = dynamic_cast<IRBinary*>(value.get())) {
            print_binary(os, bin, indent);
        } else if (auto* u = dynamic_cast<IRUnary*>(value.get())) {
            print_unary(os, u, indent);
        } else if (auto* fc = dynamic_cast<IRFunctionCall*>(value.get())) {
            print_function_call(os, fc, indent);
        } else {
            print_indent(os, indent);
            os << "<Unknown IRValue>";
        }
    }

    inline void print_node(
        std::ostream& os,
        IRInstruction* node,
        size_t indent
    ) {
        if (auto* decl = dynamic_cast<IRVariableDecl*>(node)) {
            print_variable_decl(os, decl, indent);
        } else if (auto* assign = dynamic_cast<IRAssignment*>(node)) {
            print_assignment(os, assign, indent);
        } else if (auto* ret = dynamic_cast<IRReturn*>(node)) {
            print_return(os, ret, indent);
        } else if (auto* fc = dynamic_cast<IRFunctionCall*>(node)) {
            print_function_call(os, fc, indent);
        } else if (auto* bin = dynamic_cast<IRBinary*>(node)) {
            print_binary(os, bin, indent);
        } else if (auto* u = dynamic_cast<IRUnary*>(node)) {
            print_unary(os, u, indent);
        } else if (auto* ifstmt = dynamic_cast<IRIf*>(node)) {
            print_if(os, ifstmt, indent);
        } else if (auto* whilestmt = dynamic_cast<IRWhile*>(node)) {
            print_while(os, whilestmt, indent);
        } else {
            print_indent(os, indent);
            os << "<Unknown IRInstruction>\n";
        }
    }

} // namespace cherry::ir::printer

#endif // IR_PRINTER_HPP
