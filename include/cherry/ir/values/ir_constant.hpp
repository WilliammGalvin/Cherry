#ifndef IRCONSTANT_HPP
#define IRCONSTANT_HPP

#include <string>
#include <utility>
#include <variant>

#include "ir_value.hpp"
#include "cherry/ir/ir_type.hpp"

namespace cherry::ir {

    struct IRConstant final : IRValue {
        using ConstantValue = std::variant<long long, double, std::string, bool>;

        ConstantValue value;
        IRType type;

        IRConstant(ConstantValue value, const IRType type)
            : value(std::move(value)), type(type) {}
    };

}

#endif //IRCONSTANT_HPP
