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

        void print(std::ostream& os, int indent = 0) const override {
            os << "Program:\n";
            print_indent(os, indent + 1);
            os << "Statements:\n";
            for (const auto& stmt : stmts) {
                if (stmt) {
                    stmt->print(os, indent + 2);
                } else {
                    os << "null\n";
                }
            }
        }
    };

}

#endif //PROGRAM_HPP
