#ifndef FUNCTION_DECL_HPP
#define FUNCTION_DECL_HPP
#include <memory>
#include <string>
#include <vector>

#include "statement.hpp"
#include "cherry/ast/expr/identifier.hpp"

namespace cherry::ast {

    struct Param {
        Type type;
        std::string name;

        Param(const Type type, std::string name)
            : type(type), name(std::move(name)) {}
    };

    struct FunctionDecl final : Statement {
        std::unique_ptr<IdentifierExpr> identifier;
        std::vector<Param> params;
        Type return_type;
        std::vector<std::unique_ptr<Statement>> body;

        FunctionDecl(std::unique_ptr<IdentifierExpr> identifier, std::vector<Param> params,
                     const Type return_type, std::vector<std::unique_ptr<Statement>> body)
            : identifier(std::move(identifier)), params(std::move(params)), return_type(return_type),
              body(std::move(body)) {}
    };

}

#endif //FUNCTION_DECL_HPP
