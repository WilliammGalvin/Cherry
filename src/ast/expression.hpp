#ifndef EXPRESSION_HPP
#define EXPRESSION_HPP

#include "cherry/ast/ast_node.hpp"

namespace cherry::ast {

    struct Expr : ASTNode {
        virtual ~Expr() = default;
    };

}

#endif //EXPRESSION_HPP
