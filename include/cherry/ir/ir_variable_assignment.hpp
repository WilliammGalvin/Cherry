#ifndef IR_ASSIGNMENT_HPP
#define IR_ASSIGNMENT_HPP
#include <string>

#include "ir_node.hpp"
#include "cherry/ast/types.hpp"

namespace cherry::ir {

    struct IRVariableAssignment final : IRNode {
        std::string name;
        IRValue* value;

        IRVariableAssignment(std::string name, IRValue* value)
            : name(std::move(name)), value(value) {}
    };

}

#endif //IR_ASSIGNMENT_HPP
