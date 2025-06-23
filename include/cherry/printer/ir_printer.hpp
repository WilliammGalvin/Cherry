#ifndef IR_PRINTER_HPP
#define IR_PRINTER_HPP
#include <ostream>
#include <typeindex>

#include "cherry/ir/ir_function_decl.hpp"
#include "cherry/ir/ir_program.hpp"
#include "cherry/ir/ir_values.hpp"
#include "cherry/ir/ir_variable_assignment.hpp"
#include "cherry/ir/ir_variable_decl.hpp"

#define DISPATCHER_ENTRY(TYPE, FUNC) \
    { typeid(TYPE), [](ir::IRNode* node, std::ostream& os, size_t indent) { \
        FUNC(os, dynamic_cast<TYPE*>(node), indent); \
    }}

namespace cherry::ir::printer {

    inline void print_indent(std::ostream& os, size_t indent) {
        for (size_t i = 0; i < indent; ++i) os << "  ";
    }

    inline void print_node(std::ostream& os, IRNode* node, int indent = 0);

    inline void print_function_decl(std::ostream& os, IRFunctionDecl* func_decl, const int indent = 0) {
        print_indent(os, indent);
        os << "func @" << func_decl->name << ": ";
        os << var_type_to_str(func_decl->return_type) << " {\n";
        print_indent(os, indent);
        os << "params:\n";

        for (const auto& param : func_decl->params) {
            print_indent(os, indent + 1);
            os << var_type_to_str(param->type) << " %" << param->name << "\n";
        }

        os << "entry:\n";

        for (const auto& node : func_decl->body) {
            print_node(os, node, indent + 1);
        }

        print_indent(os, indent);
        os << "}\n";
    }

    inline void print_variable(std::ostream& os, IRVariable* var, const int indent = 0) {
        os << var_type_to_str(var->type);
        os << " %" << var->name;
    }

    inline void print_constant(std::ostream& os, IRConstant* const_var, const int indent = 0) {
        os << var_type_to_str(const_var->type);
        os << " " << const_var->value;
    }

    inline void print_assignment(std::ostream& os, IRVariableAssignment* assignment, const int indent = 0) {
        print_indent(os, indent);
        os << "store ";
        print_node(os, assignment->value);
        os << " into %" << assignment->name << "\n";
    }

    inline void print_declaration(std::ostream& os, IRVariableDecl* decl, const int indent = 0) {
        print_indent(os, indent);
        os << "%" << decl->name << " = alloca ";
        os << var_type_to_str(decl->type) << "\n";
    }

    inline void print_return(std::ostream& os, IRReturn* ir_return, const int indent = 0) {
        print_indent(os, indent);
        os << "ret ";
        print_node(os, ir_return->value);
        os << "\n";
    }

    inline void print_program(std::ostream& os, IRProgram* program, const int indent = 0) {
        for (const auto& node : program->nodes) {
            print_node(os, node, indent);
        }
    }

    static std::unordered_map<std::type_index, std::function<void(IRNode*, std::ostream&, size_t)>> dispatcher = {
        DISPATCHER_ENTRY(IRProgram, print_program),
        DISPATCHER_ENTRY(IRFunctionDecl, print_function_decl),
        DISPATCHER_ENTRY(IRVariableDecl, print_declaration),
        DISPATCHER_ENTRY(IRVariable, print_variable),
        DISPATCHER_ENTRY(IRConstant, print_constant),
        DISPATCHER_ENTRY(IRVariableAssignment, print_assignment),
        DISPATCHER_ENTRY(IRReturn, print_return),
    };

    inline void print_node(std::ostream& os, IRNode* node, const int indent) {
        bool found_match = false;

        for (const auto& [type_id, handler] : dispatcher) {
            if (type_id == typeid(*node)) {
                handler(node, os, indent);
                found_match = true;
                break;
            }
        }

        if (!found_match) {
            os << "Node Printer Unkown\n";
        }
    }
    
}

#endif //IR_PRINTER_HPP
