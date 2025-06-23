#ifndef PRIVATE_BLOCK_HPP
#define PRIVATE_BLOCK_HPP
#include <memory>
#include <vector>

#include "statement.hpp"

namespace cherry::ast {

    struct PublicBlock final : Statement {
        std::vector<std::unique_ptr<Statement>> body;

        explicit PublicBlock(std::vector<std::unique_ptr<Statement>>&& body)
            : body(std::move(body)) {}
    };

    struct PrivateBlock final : Statement {
        std::vector<std::unique_ptr<Statement>> body;

        explicit PrivateBlock(std::vector<std::unique_ptr<Statement>>&& body)
            : body(std::move(body)) {}
    };

}

#endif //PRIVATE_BLOCK_HPP
