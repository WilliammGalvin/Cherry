#include "cherry/lexer/lexer.hpp"

#include <iostream>

#include "cherry/parser/parser.hpp"

int main() {
    cherry::lexer::Lexer lexer;
    const auto tokens = lexer.lex_file("test.ch");

    for (const auto& token : tokens) {
        std::cout << token.to_str() << std::endl;
    }

    std::cout << "~~~~~~~~~~~~" << std::endl;

    cherry::parser::Parser parser(tokens);
    const auto program = parser.parse_program();
    program->print(std::cout);

    return 0;
}
