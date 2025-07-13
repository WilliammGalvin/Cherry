#ifndef IR_FOR_HPP
#define IR_FOR_HPP

#include <memory>
#include <vector>

#include "ir_assignment.hpp"
#include "ir_instruction.hpp"
#include "cherry/ir/values/ir_value.hpp"

namespace cherry::ir {

    struct IRFor final : IRInstruction {
        std::unique_ptr<IRInstruction> init;
        std::unique_ptr<IRValue> condition;
        std::unique_ptr<IRAssignment> increment;
        std::vector<std::unique_ptr<IRInstruction>> body;

        IRFor(
            std::unique_ptr<IRInstruction> init,
            std::unique_ptr<IRValue> condition,
            std::unique_ptr<IRAssignment> increment,
            std::vector<std::unique_ptr<IRInstruction>>&& body
        ) :
            init(std::move(init)),
            condition(std::move(condition)),
            increment(std::move(increment)),
            body(std::move(body)) {}
    };

}

#endif //IR_FOR_HPP
