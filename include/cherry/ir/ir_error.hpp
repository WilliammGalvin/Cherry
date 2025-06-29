#ifndef LEXERROR_HPP
#define LEXERROR_HPP

#include <exception>
#include <string>

namespace cherry::ir {

    class IRError final : public std::exception {
        std::string message;

    public:
        explicit IRError(std::string msg);

        [[nodiscard]] const char* what() const noexcept override;
    };

}

#endif //LEXERROR_HPP