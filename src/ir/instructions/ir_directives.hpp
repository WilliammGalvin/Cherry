#ifndef IR_DIRECTIVES_HPP
#define IR_DIRECTIVES_HPP
#include <string>

namespace cherry::ir {

    struct IRDirective : IRInstruction {};

    struct IRSysCallDirective final : IRDirective {
        std::string call_name;
        std::string content;

        IRSysCallDirective(std::string call_name, std::string content)
            : call_name(std::move(call_name)), content(std::move(content)) {}
    };

}

#endif //IR_DIRECTIVES_HPP
