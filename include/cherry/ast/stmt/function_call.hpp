#ifndef FUNCTION_CALL_HPP
#define FUNCTION_CALL_HPP
#include <memory>
#include <vector>

#include "statement.hpp"
#include "cherry/ast/expr/expression.hpp"
#include "cherry/ast/expr/identifier.hpp"

namespace cherry::ast {

    struct FunctionCallStatement final : Statement {
        std::string callee;
        std::vector<std::unique_ptr<Expr>> args;

        FunctionCallStatement(std::string callee, std::vector<std::unique_ptr<Expr>> args)
            : callee(std::move(callee)), args(std::move(args)) {}
    };

}

#endif //FUNCTION_CALL_HPP
