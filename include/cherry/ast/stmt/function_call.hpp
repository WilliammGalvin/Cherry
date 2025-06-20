#ifndef FUNCTION_CALL_HPP
#define FUNCTION_CALL_HPP
#include <memory>
#include <vector>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct FunctionCallStatement final : Statement {
        std::string callee;
        std::vector<std::unique_ptr<Expr>> args;

        FunctionCallStatement(std::string callee, std::vector<std::unique_ptr<Expr>>&& args)
            : callee(std::move(callee)), args(std::move(args)) {}

        void print(std::ostream& os, int indent) const override {
            os << "FunctionCallStatement: " << callee << "\n";
            print_indent(os, indent + 1);
            os << "Arguments:\n";
            for (const auto& arg : args) {
                print_indent(os, indent + 2);
                os << "- ";
                if (arg) {
                    arg->print(os, indent + 2);
                } else {
                    os << "null\n";
                }
            }
        }
    };

}

#endif //FUNCTION_CALL_HPP
