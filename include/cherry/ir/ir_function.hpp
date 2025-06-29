#ifndef IR_FUNCTION_DECL_HPP
#define IR_FUNCTION_DECL_HPP
#include "ir_param.hpp"
#include "instructions/ir_instruction.hpp"

namespace cherry::ir {

    struct IRFunction {
        std::string name;
        std::vector<IRParam> params;
        IRType return_type;
        std::vector<std::unique_ptr<IRInstruction>> body;

        IRFunction(
            std::string name,
            std::vector<IRParam>&& params,
            const IRType return_type,
            std::vector<std::unique_ptr<IRInstruction>>&& body
        ) :
            name(std::move(name)),
            params(std::move(params)),
            return_type(return_type),
            body(std::move(body)) {}
    };

}

#endif //IR_FUNCTION_DECL_HPP
