#ifndef DECLARATION_HPP
#define DECLARATION_HPP

#include <memory>

#include "statement.hpp"
#include <string>

#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct Declaration final : Statement {
        bool is_const;
        std::string name;
        std::unique_ptr<Expr> initializer;
        std::string explicit_type;

        Declaration(const bool is_const, std::string name, std::unique_ptr<Expr> initializer,
                    std::string explicit_type = "")
            : is_const(is_const), name(std::move(name)), initializer(std::move(initializer)),
              explicit_type(std::move(explicit_type)) {}

        void print(std::ostream& os, int indent) const override {
            os << "Declaration:\n";
            print_indent(os, indent + 1);
            os << "Name: " << name << "\n";
            print_indent(os, indent + 1);
            os << "Is Const: " << (is_const ? "true" : "false") << "\n";
            if (!explicit_type.empty()) {
                print_indent(os, indent + 1);
                os << "Explicit Type: " << explicit_type << "\n";
            }
            print_indent(os, indent + 1);
            os << "Initializer:\n";
            if (initializer) {
                initializer->print(os, indent + 2);
            } else {
                os << "null\n";
            }
        }
    };

}

#endif //DECLARATION_HPP
