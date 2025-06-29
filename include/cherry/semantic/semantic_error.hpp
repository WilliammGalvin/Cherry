#ifndef PARSE_ERROR_HPP
#define PARSE_ERROR_HPP

#include <string>
#include <exception>

namespace cherry::semantic {

    class SemanticError final : public std::exception {
        std::string message;

    public:
        explicit SemanticError(std::string msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

}

#endif //PARSE_ERROR_HPP
