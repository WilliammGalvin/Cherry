#ifndef TOKEN_TYPE_HPP
#define TOKEN_TYPE_HPP

#include <string>

namespace cherry::lexer {

    enum TokenType {
        IDENTIFIER,

        INTEGER_LITERAL,
        FLOAT_LITERAL,
        STRING_LITERAL,
        BOOLEAN_LITERAL_TRUE,
        BOOLEAN_LITERAL_FALSE,

        KEYWORD_PUBLIC,
        KEYWORD_PRIVATE,
        KEYWORD_CONST,
        KEYWORD_INT,
        KEYWORD_FLOAT,
        KEYWORD_STRING,
        KEYWORD_BOOL,
        KEYWORD_IF,
        KEYWORD_ELSE,
        KEYWORD_WHILE,
        KEYWORD_FOR,
        KEYWORD_FUNC,
        KEYWORD_RETURN,
        KEYWORD_VOID,

        SEMI_COLON,
        COLON,
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
        PLUS_EQUAL,
        MINUS_EQUAL,
        STAR_EQUAL,
        SLASH_EQUAL,
        PERCENT_EQUAL,

        LINE_END,
        END_OF_FILE,
    };

    std::string token_type_to_str(TokenType type);

}

#endif //TOKEN_TYPE_HPP