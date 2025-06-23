#ifndef LITERAL_HPP
#define LITERAL_HPP
#include <string>
#include <utility>

#include "expression.hpp"

namespace cherry::ast {

    struct IntegerLiteral final : Expr {
        long long value;

        explicit IntegerLiteral(const long long value) : value(value) {}
    };

    struct FloatLiteral final : Expr {
        double value;

        explicit FloatLiteral(const double value) : value(value) {}
    };

    struct StringLiteral final : Expr {
        std::string value;

        explicit StringLiteral(std::string  value) : value(std::move(value)) {}
    };

    struct BooleanLiteral final : Expr {
        bool value;

        explicit BooleanLiteral(const bool value) : value(value) {}
    };

}

#endif //LITERAL_HPP
