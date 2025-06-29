#ifndef IR_IDENTIFIER_HPP
#define IR_IDENTIFIER_HPP
#include <string>

#include "ir_value.hpp"

namespace cherry::ir {

    struct IRIdentifier final : IRValue {
        std::string name;

        explicit IRIdentifier(std::string name) : name(std::move(name)) {}
    };

}

#endif //IR_IDENTIFIER_HPP
