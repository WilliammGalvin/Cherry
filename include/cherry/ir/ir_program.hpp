#ifndef IR_PROGRAM_HPP
#define IR_PROGRAM_HPP
#include <vector>

#include "ir_node.hpp"

namespace cherry::ir {

    struct IRProgram final : IRNode {
        std::vector<IRNode*> nodes;

        explicit IRProgram(const std::vector<IRNode*>&& nodes)
            : nodes(nodes) {}
    };

}

#endif //IR_PROGRAM_HPP
