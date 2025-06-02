#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "token_type.hpp"

#include <string>

namespace cherry::lexer {

    struct Token {
        TokenType type;
        std::string value;

        Token(TokenType type, std::string value);
        explicit Token(TokenType type);

        [[nodiscard]] std::string to_str() const;
    };

}

#endif //TOKEN_HPP
