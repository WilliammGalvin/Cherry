#ifndef IR_RETURN_HPP
#define IR_RETURN_HPP
#include "ir_instruction.hpp"
#include "cherry/ir/values/ir_value.hpp"

namespace cherry::ir {

    struct IRReturn final : IRInstruction {
        std::unique_ptr<IRValue> value;

        explicit IRReturn(std::unique_ptr<IRValue> value)
            : value(std::move(value)) {}
    };

}

#endif //IR_RETURN_HPP
