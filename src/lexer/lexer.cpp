#include "cherry/lexer/lexer.hpp"
#include "cherry/lexer/lex_error.hpp"
#include "cherry/lexer/token_type.hpp"

#include <fstream>
#include <iostream>
#include <unordered_map>

using namespace cherry::lexer;

static bool is_alpha_or_underscore(const char c) {
    return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || c == '_';
}

static bool is_alphanumeric_or_underscore(const char c) {
    return ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) || c == '_';
}

char Lexer::peek(const size_t pos) const {
    if (index + pos >= line_source.length()) return '\0';
    return line_source.at(index + pos);
}

char Lexer::consume() {
    if (index >= line_source.length()) {
        throw LexError("Unexpected end of input.");
    }

    return line_source[index++];
}

void Lexer::advance(const size_t pos) {
    if (index + pos > line_source.length()) {
        throw LexError("Unexpected end of input.");
    }

    index += pos;
}

bool Lexer::is_empty() const {
    return index >= line_source.length();
}

bool Lexer::match_front(const std::string& str) const {
    return std::string_view(line_source).substr(index).starts_with(str);
}

bool Lexer::match_keyword() {
    if (!is_alpha_or_underscore(peek()))
        return false;

    const std::unordered_map<std::string, TokenType> keywords = {
        { "public", KEYWORD_PUBLIC },
        { "private", KEYWORD_PRIVATE },
        { "const", KEYWORD_CONST },
        { "int", KEYWORD_INT },
        { "string", KEYWORD_STRING },
        { "float", KEYWORD_FLOAT },
        { "bool", KEYWORD_BOOL },
        { "if", KEYWORD_IF },
        { "else", KEYWORD_ELSE },
        { "while", KEYWORD_WHILE },
        { "for", KEYWORD_FOR },
        { "func", KEYWORD_FUNC },
        { "return", KEYWORD_RETURN },
        { "void", KEYWORD_VOID },

        { "true", BOOLEAN_LITERAL_TRUE },
        { "false", BOOLEAN_LITERAL_FALSE },
    };

    std::string longest;
    for (const auto& [key, value] : keywords) {
        if (match_front(key) && key.length() > longest.length()) {
            longest = key;
        }
    }

    if (longest.empty()) {
        return false;
    }

    advance(longest.length());
    tokens.emplace_back(keywords.at(longest), longest);
    return true;
}

bool Lexer::match_symbol() {
    const std::vector<std::pair<std::string, TokenType>> symbols = {
        {";", SEMI_COLON},
        {":", COLON},
        {",", COMMA},
        {"(", LEFT_PAREN},
        {")", RIGHT_PAREN},
        {"{", LEFT_BRACE},
        {"}", RIGHT_BRACE},
        {">=", GREATER_THAN_OR_EQUAL},
        {"<=", LESSER_THAN_OR_EQUAL},
        {">", GREATER_THAN},
        {"<", LESSER_THAN},
        {"+=", PLUS_EQUAL},
        {"-=", MINUS_EQUAL},
        {"*=", STAR_EQUAL},
        {"/=", SLASH_EQUAL},
        {"%=", PERCENT_EQUAL},
        {"+", PLUS},
        {"-", MINUS},
        {"*", STAR},
        {"/", SLASH},
        {"%", PERCENT},
        {"==", DOUBLE_EQUAL},
        {"=", EQUAL},
        {"!=", BANG_EQUAL},
        {"!", BANG},
        {"||", OR},
        {"&&", AND},
    };

    for (const auto& [key, value] : symbols) {
        if (!match_front(key)) continue;

        advance(key.length());
        tokens.emplace_back(value, key);
        return true;
    }

    return false;
}

bool Lexer::match_identifier() {
    std::string buffer;

    if (!is_alpha_or_underscore(peek()))
        return false;

    buffer += consume();

    while (!is_empty() && is_alphanumeric_or_underscore(peek())) {
        buffer += consume();
    }

    tokens.emplace_back(IDENTIFIER, buffer);
    return true;
}

bool Lexer::match_number_literal() {
    std::string buffer;
    bool is_float = false;

    while (!is_empty()) {
        if (std::isdigit(peek())) {
            buffer += consume();
            continue;
        }

        if (peek() == '.' && !is_float) {
            if (buffer.empty())
                buffer += '0';

            buffer += consume();
            is_float = true;
            continue;
        }

        if ((peek() == 'f' || peek() == 'F') && !buffer.empty()) {
            is_float = true;
            advance(1);
            break;
        }

        break;
    }

    if (!buffer.empty()) {
        const TokenType type = is_float ? FLOAT_LITERAL : INTEGER_LITERAL;
        tokens.emplace_back(type, buffer);
        return true;
    }

    return false;
}

bool Lexer::match_string_literal() {
    std::string buffer;

    if (peek() != '"') {
        return false;
    }

    advance(1);

    while (!is_empty()) {
        if (peek() == '"') {
            advance(1);
            tokens.emplace_back(STRING_LITERAL, buffer);
            return true;
        }

        buffer += consume();
    }

    return false;
}

void Lexer::lex_line() {
    while (!is_empty()) {
        if (std::isspace(peek())) {
            advance(1);
            continue;
        }

        if (match_front("*/")) {
            in_block_comment = false;
            advance(2);
            continue;
        }

        if (in_block_comment) {
            advance(1);
            continue;
        }

        if (match_front("/*")) {
            in_block_comment = true;
            advance(2);
            continue;
        }

        if (match_front("//")) {
            line_source.clear();
            break;
        }

        if (
               match_symbol()
            || match_keyword()
            || match_number_literal()
            || match_string_literal()
            || match_identifier()
        ) continue;

        throw LexError("Unable to determine token for source:\n" + line_source + "\n");
    }
}

std::vector<Token> Lexer::lex_file(const std::string& file_name) {
    std::ifstream file(file_name);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open source file.");
    }

    while (std::getline(file, line_source)) {
        lex_line();
        tokens.emplace_back(LINE_END);
        line_source.clear();
        index = 0;
    }

    tokens.emplace_back(END_OF_FILE);
    return tokens;
}