#ifndef IR_VARIABLE_DECL_HPP
#define IR_VARIABLE_DECL_HPP
#include <memory>
#include <string>

#include "ir_instruction.hpp"
#include "../values/ir_value.hpp"
#include "cherry/ir/ir_type.hpp"

namespace cherry::ir {

    struct IRVariableDecl final : IRInstruction {
        std::string name;
        IRType type;
        std::unique_ptr<IRValue> initializer;

        IRVariableDecl(
            std::string name,
            const IRType type,
            std::unique_ptr<IRValue> initializer
        ) :
            name(std::move(name)),
            type(type),
            initializer(std::move(initializer)) {}
    };

}

#endif //IR_VARIABLE_DECL_HPP
