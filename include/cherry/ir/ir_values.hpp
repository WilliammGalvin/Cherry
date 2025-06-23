#ifndef IR_VALUES_HPP
#define IR_VALUES_HPP
#include <string>

#include "ir_node.hpp"
#include "cherry/ast/types.hpp"

namespace cherry::ir {

    struct IRValue : IRNode {
        ast::Type type;

        explicit IRValue(const ast::Type type)
            : type(type) {}
    };

    struct IRVariable final : IRValue {
        std::string name;
        IRValue* value;

        IRVariable(std::string name, const ast::Type type, IRValue* value)
            : IRValue(type), name(std::move(name)), value(value) {}
    };

    struct IRConstant final : IRValue {
        std::string value;

        IRConstant(const ast::Type type, std::string value)
            : IRValue(type), value(std::move(value)) {}
    };

}

#endif //IR_VALUES_HPP
