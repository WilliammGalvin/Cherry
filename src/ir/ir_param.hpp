#ifndef IR_PARAM_HPP
#define IR_PARAM_HPP

#include <string>
#include "ir_type.hpp"

namespace cherry::ir {

    struct IRParam {
        std::string name;
        IRType type;

        IRParam(std::string name, const IRType type)
            : name(std::move(name)), type(type) {}
    };

}

#endif //IR_PARAM_HPP
