#ifndef STATEMENT_HPP
#define STATEMENT_HPP
#include "cherry/ast/ast_node.hpp"

namespace cherry::ast {

    struct Statement : ASTNode {
        ~Statement() override = default;
    };

}

#endif //STATEMENT_HPP
