#ifndef LITERAL_HPP
#define LITERAL_HPP
#include <string>
#include <utility>

#include "expression.hpp"

namespace cherry::ast {

    struct IntegerLiteral final : Expr {
        long long value;

        explicit IntegerLiteral(const long long value) : value(value) {}

        void print(std::ostream& os, int indent) const override {
            os << "IntegerLiteral: " << value << "\n";
            print_indent(os, indent + 1);
            os << "Value: " << value << "\n";
        }
    };

    struct FloatLiteral final : Expr {
        double value;

        explicit FloatLiteral(const double value) : value(value) {}

        void print(std::ostream& os, int indent) const override {
            os << "FloatLiteral: " << value << "\n";
            print_indent(os, indent + 1);
            os << "Value: " << value << "\n";
        }
    };

    struct StringLiteral final : Expr {
        std::string value;

        explicit StringLiteral(std::string  value) : value(std::move(value)) {}

        void print(std::ostream& os, int indent) const override {
            os << "StringLiteral: \"" << value << "\"\n";
            print_indent(os, indent + 1);
            os << "Value: \"" << value << "\"\n";
        }
    };

    struct BooleanLiteral final : Expr {
        bool value;

        explicit BooleanLiteral(const bool value) : value(value) {}

        void print(std::ostream& os, int indent) const override {
            os << "BooleanLiteral: " << (value ? "true" : "false") << "\n";
            print_indent(os, indent + 1);
            os << "Value: " << (value ? "true" : "false") << "\n";
        }
    };

}

#endif //LITERAL_HPP
