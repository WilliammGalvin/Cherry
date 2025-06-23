#ifndef IR_FUNCTION_DECL_HPP
#define IR_FUNCTION_DECL_HPP
#include <string>

#include "ir_node.hpp"
#include "cherry/ast/types.hpp"

namespace cherry::ir {

    struct IRParam final : IRNode {
        std::string name;
        ast::Type type;

        IRParam(std::string name, const ast::Type type)
            : name(std::move(name)), type(type) {}
    };

    struct IRFunctionDecl final : IRNode {
        std::string name;
        ast::Type return_type;
        std::vector<IRParam*> params;
        std::vector<IRNode*> body;

        IRFunctionDecl(std::string name, const ast::Type return_type, std::vector<IRParam*>&& params, std::vector<IRNode*>&& body)
            : name(std::move(name)), return_type(return_type), params(params), body(body) {}
    };

}

#endif //IR_FUNCTION_DECL_HPP
