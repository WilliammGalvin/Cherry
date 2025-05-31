#ifndef LEXERROR_HPP
#define LEXERROR_HPP

#include <exception>
#include <string>

namespace lexer {

    class LexError final : public std::exception {
        std::string message;

    public:
        explicit LexError(std::string msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

}

#endif //LEXERROR_HPP