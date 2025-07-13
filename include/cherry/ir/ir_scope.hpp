#ifndef IR_SCOPES_HPP
#define IR_SCOPES_HPP
#include <memory>
#include <vector>

#include "ir_function.hpp"

namespace cherry::ir {

    enum ScopeVisibility {
        PUBLIC, PRIVATE
    };

    struct IRScope {
        ScopeVisibility visibility;
        std::vector<std::unique_ptr<IRFunction>> functions;

        IRScope(const ScopeVisibility visibility, std::vector<std::unique_ptr<IRFunction>>&& functions)
            : visibility(visibility), functions(std::move(functions)) {}
    };

}

#endif //IR_SCOPES_HPP
