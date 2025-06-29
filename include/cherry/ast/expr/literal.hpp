#ifndef LITERAL_HPP
#define LITERAL_HPP
#include <string>
#include <utility>
#include <variant>

#include "expression.hpp"
#include "cherry/ast/types.hpp"

namespace cherry::ast {

    struct Literal : Expr {
        using ValueType = std::variant<std::string, double, long long, bool>;

        Type type;
        ValueType value;

        Literal(const Type type, ValueType value) : type(type), value(std::move(value)) {};
    };

    struct IntegerLiteral final : Literal {
        explicit IntegerLiteral(const long long value) : Literal(INT, value) {}
    };

    struct FloatLiteral final : Literal {
        explicit FloatLiteral(const double value) : Literal(FLOAT, value) {}
    };

    struct StringLiteral final : Literal {
        explicit StringLiteral(std::string value) : Literal(STRING, std::move(value)) {}
    };

    struct BooleanLiteral final : Literal {
        explicit BooleanLiteral(const bool value) : Literal(BOOL, value) {}
    };

}

#endif //LITERAL_HPP
