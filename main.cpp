#include <iostream>

#include "lexer/include/lexer.hpp"

int main() {
    lexer::Lexer lexer;
    const auto tokens = lexer.lex_file("test.ch");

    for (const auto& token : tokens) {
        std::cout << token.to_str() << std::endl;
    }

    return 0;
}
