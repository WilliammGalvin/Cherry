#ifndef IR_PROGRAM_HPP
#define IR_PROGRAM_HPP

#include <vector>
#include "ir_scope.hpp"

namespace cherry::ir {

    struct IRProgram {
        std::vector<std::unique_ptr<IRScope>> scopes;

        explicit IRProgram(std::vector<std::unique_ptr<IRScope>>&& scopes)
            : scopes(std::move(scopes)) {}
    };

}

#endif //IR_PROGRAM_HPP
