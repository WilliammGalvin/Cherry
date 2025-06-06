#include "cherry/lexer/token_type.hpp"

namespace cherry::lexer {

    std::string token_type_to_str(const TokenType type) {
        switch (type) {
            case IDENTIFIER: return "IDENTIFIER";
            case INTEGER_LITERAL: return "INTEGER_LITERAL";
            case FLOAT_LITERAL: return "FLOAT_LITERAL";
            case STRING_LITERAL: return "STRING_LITERAL";
            case BOOLEAN_LITERAL_TRUE: return "BOOLEAN_LITERAL_TRUE";
            case BOOLEAN_LITERAL_FALSE: return "BOOLEAN_LITERAL_FALSE";

            case KEYWORD_CONST: return "KEYWORD_CONST";
            case KEYWORD_VAR: return "KEYWORD_VAR";
            case KEYWORD_INT: return "KEYWORD_INT";
            case KEYWORD_STRING: return "KEYWORD_STRING";
            case KEYWORD_FLOAT: return "KEYWORD_FLOAT";
            case KEYWORD_BOOL: return "KEYWORD_BOOL";
            case KEYWORD_IF: return "KEYWORD_IF";
            case KEYWORD_ELSE: return "KEYWORD_ELSE";
            case KEYWORD_WHILE: return "KEYWORD_WHILE";
            case KEYWORD_FUNC: return "KEYWORD_FUNC";
            case KEYWORD_RETURN: return "KEYWORD_RETURN";
            case KEYWORD_VOID: return "KEYWORD_VOID";

            case BUILTIN_PRINT: return "BUILTIN_PRINT";
            case BUILTIN_PRINTLN: return "BUILTIN_PRINTLN";

            case COLON: return "COLON";
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
            case BANG: return "BANG";
            case BANG_EQUAL: return "BANG_EQUAL";
            case OR: return "OR";
            case AND: return "AND";

            case LINE_END: return "LINE_END";
            case END_OF_FILE: return "END_OF_FILE";

            default: return "UNKNOWN_TOKEN";
        }
    }

}