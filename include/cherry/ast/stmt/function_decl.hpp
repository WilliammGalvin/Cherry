#ifndef FUNCTION_DECL_HPP
#define FUNCTION_DECL_HPP
#include <memory>
#include <string>
#include <vector>

#include "statement.hpp"
#include "cherry/ast/expr/identifier.hpp"

namespace cherry::ast {

    struct Param {
        std::string type_name;
        std::string name;

        Param(std::string type_name, std::string name)
            : type_name(std::move(type_name)), name(std::move(name)) {}
    };

    struct FunctionDecl final : Statement {
        std::unique_ptr<IdentifierExpr> identifier;
        std::vector<Param> params;
        std::string return_type;
        std::vector<std::unique_ptr<Statement>> body;

        FunctionDecl(std::unique_ptr<IdentifierExpr> identifier, std::vector<Param> params,
                     std::string return_type, std::vector<std::unique_ptr<Statement>> body)
            : identifier(std::move(identifier)), params(std::move(params)), return_type(std::move(return_type)),
              body(std::move(body)) {}

        void print(std::ostream& os, int indent) const override {
            os << "FunctionDecl:\n";
            print_indent(os, indent + 1);
            os << "Identifier: ";
            if (identifier) {
                identifier->print(os, indent + 2);
            } else {
                os << "null\n";
            }
            print_indent(os, indent + 1);
            os << "Parameters:\n";
            for (const auto& param : params) {
                print_indent(os, indent + 2);
                os << "- Type: " << param.type_name << ", Name: " << param.name << "\n";
            }
            print_indent(os, indent + 1);
            os << "Return Type: " << return_type << "\n";
            print_indent(os, indent + 1);
            os << "Body:\n";
            for (const auto& stmt : body) {
                if (stmt) {
                    stmt->print(os, indent + 2);
                } else {
                    os << "null\n";
                }
            }
        }
    };

}

#endif //FUNCTION_DECL_HPP
