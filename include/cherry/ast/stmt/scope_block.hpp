#ifndef PRIVATE_BLOCK_HPP
#define PRIVATE_BLOCK_HPP
#include <memory>
#include <vector>

#include "statement.hpp"

namespace cherry::ast {

    enum ScopeVisibility {
        PUBLIC, PRIVATE
    };

    struct VisibilityScope final : Statement {
        ScopeVisibility visibility;
        std::vector<std::unique_ptr<Statement>> body;

        VisibilityScope(const ScopeVisibility visibility, std::vector<std::unique_ptr<Statement>>&& body)
            : visibility(visibility), body(std::move(body)) {}
    };

}

#endif //PRIVATE_BLOCK_HPP
