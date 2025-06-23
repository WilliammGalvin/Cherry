#ifndef IDENTIFIER_HPP
#define IDENTIFIER_HPP
#include <string>

#include "expression.hpp"

namespace cherry::ast {

    struct IdentifierExpr final : Expr {
        std::string name;

        explicit IdentifierExpr(std::string name) : name(std::move(name)) {}
    };

}

#endif //IDENTIFIER_HPP
