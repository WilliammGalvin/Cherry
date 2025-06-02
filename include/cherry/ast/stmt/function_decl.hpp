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
    };

}

#endif //FUNCTION_DECL_HPP
