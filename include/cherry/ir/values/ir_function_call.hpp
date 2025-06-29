#ifndef IR_FUNCTION_CALL_HPP
#define IR_FUNCTION_CALL_HPP
#include <memory>
#include <string>
#include <vector>

#include "ir_value.hpp"

namespace cherry::ir {

    struct IRFunctionCall final : IRValue {
        std::string callee;
        std::vector<std::unique_ptr<IRValue>> arguments;

        IRFunctionCall(std::string callee, std::vector<std::unique_ptr<IRValue>>&& arguments)
            : callee(std::move(callee)), arguments(std::move(arguments)) {}
    };

}

#endif //IR_FUNCTION_CALL_HPP
