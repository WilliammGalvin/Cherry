#ifndef IR_ASSIGNMENT_HPP
#define IR_ASSIGNMENT_HPP

#include <string>

#include "ir_instruction.hpp"
#include "cherry/ir/values/ir_value.hpp"

namespace cherry::ir {

    struct IRAssignment final : IRInstruction {
        std::string target;
        std::unique_ptr<IRValue> value;

        IRAssignment(std::string target, std::unique_ptr<IRValue> value)
            : target(std::move(target)), value(std::move(value)) {}
    };

}

#endif //IR_ASSIGNMENT_HPP
