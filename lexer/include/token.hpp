#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

namespace lexer {

    enum TokenType {
        IDENTIFIER,

        INTEGER_LITERAL,
        FLOAT_LITERAL,
        STRING_LITERAL,
        BOOLEAN_LITERAL,

        KEYWORD_CONST,
        KEYWORD_DECLARE,
        KEYWORD_TYPE,
        KEYWORD_EQUAL,
        KEYWORD_DOUBLE_EQUAL,
        KEYWORD_NOT,
        KEYWORD_ADD,
        KEYWORD_MINUS,
        KEYWORD_MULTIPLY,
        KEYWORD_DIVIDE,
        KEYWORD_MOD,
        KEYWORD_NEGATIVE,
        KEYWORD_IF,
        KEYWORD_WHILE,
        KEYWORD_LOOP,
        KEYWORD_OR,
        KEYWORD_AND,

        SEMI_COLON,
        COMMA,
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        GREATER_THAN,
        LESSER_THAN,
        GREATER_THAN_OR_EQUAL,
        LESSER_THAN_OR_EQUAL,
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PERCENT,
        EQUAL,
        DOUBLE_EQUAL,
        OR,
        AND,
        BANG,
        BANG_EQUAL,

        LINE_END,
        END_OF_FILE,
    };

    struct Token {
        TokenType type;
        std::string value;

        Token(TokenType type, std::string value);
        explicit Token(TokenType type);

        [[nodiscard]] std::string to_str() const;
    };

    std::string token_type_to_str(TokenType type);

}

#endif //TOKEN_HPP
