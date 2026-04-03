#ifndef PROGRAM_HPP
#define PROGRAM_HPP
#include <memory>
#include <vector>

#include "ast_node.hpp"
#include "stmt/statement.hpp"

namespace cherry::ast {

    struct Program final : ASTNode {
        std::vector<std::unique_ptr<Statement>> stmts;

        explicit Program(std::vector<std::unique_ptr<Statement>>&& stmts) : stmts(std::move(stmts)) {}
    };

}

#endif //PROGRAM_HPP
