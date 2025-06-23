#ifndef IR_RETURN_HPP
#define IR_RETURN_HPP
#include "ir_node.hpp"
#include "ir_values.hpp"

namespace cherry::ir {

    struct IRReturn final : IRNode {
        IRValue* value;

        explicit IRReturn(IRValue* value)
            : value(value) {}
    };

}

#endif //IR_RETURN_HPP
