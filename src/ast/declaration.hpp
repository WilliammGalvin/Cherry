#ifndef DECLARATION_HPP
#define DECLARATION_HPP

#include <memory>

#include "statement.hpp"
#include <string>

#include "cherry/ast/types.hpp"
#include "cherry/ast/expr/expression.hpp"

namespace cherry::ast {

    struct Declaration final : Statement {
        bool is_const;
        std::string name;
        std::unique_ptr<Expr> initializer;
        Type explicit_type;

        Declaration(const bool is_const, std::string name, std::unique_ptr<Expr> initializer, Type explicit_type)
            : is_const(is_const), name(std::move(name)), initializer(std::move(initializer)), explicit_type(explicit_type) {}
    };

}

#endif //DECLARATION_HPP
