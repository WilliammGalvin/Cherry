#ifndef PRIVATE_BLOCK_HPP
#define PRIVATE_BLOCK_HPP
#include <memory>
#include <vector>

#include "statement.hpp"

namespace cherry::ast {

    struct PublicBlock final : Statement {
        std::vector<std::unique_ptr<Statement>> body;

        PublicBlock(std::vector<std::unique_ptr<Statement>>&& body)
            : body(std::move(body)) {}

        void print(std::ostream& os, int indent) const override {
            os << "PublicBlock:\n";
            for (const auto& stmt : body) {
                print_indent(os, indent + 1);
                if (stmt) {
                    stmt->print(os, indent + 1);
                } else {
                    os << "null\n";
                }
            }
        }
    };

    struct PrivateBlock final : Statement {
        std::vector<std::unique_ptr<Statement>> body;

        PrivateBlock(std::vector<std::unique_ptr<Statement>>&& body)
            : body(std::move(body)) {}

        void print(std::ostream& os, int indent) const override {
            os << "PrivateBlock:\n";
            for (const auto& stmt : body) {
                print_indent(os, indent + 1);
                if (stmt) {
                    stmt->print(os, indent + 1);
                } else {
                    os << "null\n";
                }
            }
        }
    };

}

#endif //PRIVATE_BLOCK_HPP
