#ifndef FUNCTION_CALL_EXPR_HPP
#define FUNCTION_CALL_EXPR_HPP
#include <memory>
#include <string>
#include <vector>

#include "expression.hpp"

namespace cherry::ast {

    struct FunctionCallExpr final : Expr {
        std::string callee;
        std::vector<std::unique_ptr<Expr>> args;

        FunctionCallExpr(std::string callee, std::vector<std::unique_ptr<Expr>>&& args)
            : callee(std::move(callee)), args(std::move(args)) {}

        void print(std::ostream& os, int indent) const override {
            os << "FunctionCallExpr: " << callee << "\n";
            print_indent(os, indent + 1);
            os << "Arguments:\n";
            for (const auto& arg : args) {
                print_indent(os, indent + 2);
                os << "- ";
                arg->print(os, indent + 2);
                os << "\n";
            }
        }
    };

}

#endif //FUNCTION_CALL_EXPR_HPP
