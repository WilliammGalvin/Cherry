#ifndef DIRECTIVE_STATEMENT_HPP
#define DIRECTIVE_STATEMENT_HPP
#include <string>

#include "statement.hpp"

namespace cherry::ast {

    enum DirectiveType {
        SYS_CALL
    };

    struct DirectiveStatement : Statement {
        DirectiveType type;
        explicit DirectiveStatement(const DirectiveType type) : type(type) {}
    };

    struct SysCallDirective final : DirectiveStatement {
        std::string call_name;
        std::string content;

        SysCallDirective(std::string name, std::string content)
            : DirectiveStatement(SYS_CALL), call_name(std::move(name)), content(std::move(content)) {}
    };

}

#endif //DIRECTIVE_STATEMENT_HPP
