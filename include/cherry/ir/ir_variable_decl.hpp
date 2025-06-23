#ifndef IR_VARIABLE_DECL_HPP
#define IR_VARIABLE_DECL_HPP
#include <string>

#include "ir_node.hpp"
#include "cherry/ast/types.hpp"

namespace cherry::ir {

    struct IRVariableDecl final : IRNode {
        std::string name;
        ast::Type type;

        IRVariableDecl(std::string name, const ast::Type type)
            : name(std::move(name)), type(type) {}
    };

}

#endif //IR_VARIABLE_DECL_HPP
