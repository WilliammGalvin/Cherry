#include "../include/token.hpp"

using namespace lexer;

Token::Token(const TokenType type, std::string value)
    : type(type), value(std::move(value)) {}

Token::Token(const TokenType type)
    : type(type) {};

std::string Token::to_str() const {
    std::string str = token_type_to_str(type);

    if (!value.empty()) {
        if (type != STRING_LITERAL && type != FLOAT_LITERAL &&
            type != INTEGER_LITERAL && value.length() == 1) {
            str += "('" + value + "')";
        } else {
            str += "(" + value + ")";
        }
    }

    return str;
}

std::string lexer::token_type_to_str(const TokenType type) {
    switch (type) {
        case IDENTIFIER: return "IDENTIFIER";

        case INTEGER_LITERAL: return "INTEGER_LITERAL";
        case FLOAT_LITERAL: return "FLOAT_LITERAL";
        case STRING_LITERAL: return "STRING_LITERAL";
        case BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";

        case KEYWORD_CONST: return "KEYWORD_CONST";
        case KEYWORD_DECLARE: return "KEYWORD_DECLARE";
        case KEYWORD_TYPE: return "KEYWORD_TYPE";
        case KEYWORD_EQUAL: return "KEYWORD_EQUAL";
        case KEYWORD_DOUBLE_EQUAL: return "KEYWORD_DOUBLE_EQUAL";
        case KEYWORD_NOT: return "KEYWORD_NOT";
        case KEYWORD_ADD: return "KEYWORD_ADD";
        case KEYWORD_MINUS: return "KEYWORD_MINUS";
        case KEYWORD_MULTIPLY: return "KEYWORD_MULTIPLY";
        case KEYWORD_DIVIDE: return "KEYWORD_DIVIDE";
        case KEYWORD_MOD: return "KEYWORD_MOD";
        case KEYWORD_NEGATIVE: return "KEYWORD_NEGATIVE";
        case KEYWORD_IF: return "KEYWORD_IF";
        case KEYWORD_WHILE: return "KEYWORD_WHILE";
        case KEYWORD_LOOP: return "KEYWORD_LOOP";
        case KEYWORD_OR: return "KEYWORD_OR";
        case KEYWORD_AND: return "KEYWORD_AND";

        case SEMI_COLON: return "SEMI_COLON";
        case COMMA: return "COMMA";
        case LEFT_PAREN: return "LEFT_PAREN";
        case RIGHT_PAREN: return "RIGHT_PAREN";
        case LEFT_BRACE: return "LEFT_BRACE";
        case RIGHT_BRACE: return "RIGHT_BRACE";
        case GREATER_THAN: return "GREATER_THAN";
        case LESSER_THAN: return "LESSER_THAN";
        case GREATER_THAN_OR_EQUAL: return "GREATER_THAN_OR_EQUAL";
        case LESSER_THAN_OR_EQUAL: return "LESSER_THAN_OR_EQUAL";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case STAR: return "STAR";
        case SLASH: return "SLASH";
        case PERCENT: return "PERCENT";
        case EQUAL: return "EQUAL";
        case DOUBLE_EQUAL: return "DOUBLE_EQUAL";
        case OR: return "OR";
        case AND: return "AND";
        case BANG: return "BANG";
        case BANG_EQUAL: return "BANG_EQUAL";

        case LINE_END: return "LINE_END";
        case END_OF_FILE: return "END_OF_FILE";

        default: return "UNKNOWN";
    }
}