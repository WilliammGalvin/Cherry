#include "cherry/lexer/token.hpp"

using namespace cherry::lexer;

Token::Token(const TokenType type, std::string value)
    : type(type), value(std::move(value)) {}

Token::Token(const TokenType type)
    : type(type) {}

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
