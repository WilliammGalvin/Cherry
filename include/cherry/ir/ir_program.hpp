#ifndef IR_PROGRAM_HPP
#define IR_PROGRAM_HPP

#include <vector>

namespace cherry::ir {

    struct IRProgram {
        std::vector<std::unique_ptr<IRFunction>> functions;

        explicit IRProgram(std::vector<std::unique_ptr<IRFunction>>&& functions)
            : functions(std::move(functions)) {}
    };

}

#endif //IR_PROGRAM_HPP
