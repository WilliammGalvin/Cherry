#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"

#include <vector>

namespace cherry::lexer {

    class Lexer {
        std::vector<Token> tokens;
        std::string line_source;
        size_t index{ 0 };

        bool in_block_comment = false;

        [[nodiscard]] char peek(size_t pos = 0) const;
        char consume();
        void advance(size_t pos = 1);
        [[nodiscard]] bool is_empty() const;
        [[nodiscard]] bool match_front(const std::string& str) const;

        bool match_symbol();
        bool match_number_literal();
        bool match_string_literal();
        bool match_keyword();
        bool match_identifier();
        bool match_directive();

        void lex_line();

    public:
        std::vector<Token> lex_file(const std::string& file_name);
    };

}

#endif //LEXER_HPP
