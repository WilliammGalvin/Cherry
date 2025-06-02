#ifndef PARSE_ERROR_HPP
#define PARSE_ERROR_HPP

#include <string>
#include <exception>

namespace cherry::parser {

    class ParseError final : public std::exception {
        std::string message;

    public:
        explicit ParseError(std::string msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

}

#endif //PARSE_ERROR_HPP
