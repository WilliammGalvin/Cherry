#ifndef IR_BUILDER_HPP
#define IR_BUILDER_HPP
#include <cherry/ast/program.hpp>
#include <cherry/ast/stmt/assignment.hpp>
#include <cherry/ast/stmt/declaration.hpp>
#include <cherry/ast/stmt/function_decl.hpp>
#include <cherry/ast/stmt/return_statement.hpp>

#include "ir_function_decl.hpp"
#include "ir_program.hpp"
#include "ir_return.hpp"
#include "ir_variable_assignment.hpp"
#include "ir_variable_decl.hpp"

namespace cherry::ir {

    class IRBuilder {
        IRReturn* lower_return(ast::ReturnStatement* return_stmt);
        IRFunctionDecl* lower_function_decl(ast::FunctionDecl* func_decl);
        std::pair<IRVariableDecl*, IRVariableAssignment*> lower_variable_decl(ast::Declaration* decl);
        IRVariableAssignment* lower_variable_assignment(ast::Assignment* assignment);

    public:
        IRProgram* lower_program(ast::Program* program);
    };

}

#endif //IR_BUILDER_HPP
