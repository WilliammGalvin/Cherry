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
    };

}

#endif //DECLARATION_HPP
