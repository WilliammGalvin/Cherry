#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP
#include <string>

#include "expression.hpp"

namespace cherry::ast {

    struct IdentifierExpr final : Expr {
        std::string name;

        explicit IdentifierExpr(std::string name) : name(std::move(name)) {}

        void print(std::ostream& os, int indent) const override {
            os << "IdentifierExpr: " << name << "\n";
            print_indent(os, indent + 1);
            os << "Name: " << name << "\n";
        }
    };

}

#endif //IDENTIFIER_HPP
