#ifndef IR_WHILE_HPP
#define IR_WHILE_HPP
#include <memory>
#include <vector>

#include "ir_instruction.hpp"
#include "cherry/ir/values/ir_value.hpp"

namespace cherry::ir {

    struct IRWhile final : IRInstruction {
        std::unique_ptr<IRValue> condition;
        std::vector<std::unique_ptr<IRInstruction>> body;

        IRWhile(
            std::unique_ptr<IRValue> cond,
            std::vector<std::unique_ptr<IRInstruction>>&& body
        ) :
            condition(std::move(cond)),
            body(std::move(body)) {}
    };

}

#endif //IR_WHILE_HPP
