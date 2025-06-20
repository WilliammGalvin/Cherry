#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <ostream>

namespace cherry::ast {

    struct ASTNode {
        static void print_indent(std::ostream& os, const int indent) {
            for (int i = 0; i < indent; ++i) {
                os << "  ";
            }
        }

        virtual ~ASTNode() = default;
        virtual void print(std::ostream& os, int indent) const;
    };

}

#endif //AST_NODE_HPP
