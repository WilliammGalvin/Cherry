#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <vector>
#include <unordered_map>
#include <string>

namespace cherry::semantic {

    enum SymbolType {
        VARIABLE,
        FUNCTION,
    };

    class SymbolTable {
        std::vector<std::unordered_map<std::string, SymbolType>> scopes;

    public:
        SymbolTable();

        void enter_scope();
        void exit_scope();
        [[nodiscard]] bool declare(const std::string& name, SymbolType type);
        [[nodiscard]] bool is_declared(const std::string& name) const;
        [[nodiscard]] const SymbolType* get_type(const std::string& name) const;
    };

}

#endif //SYMBOL_TABLE_HPP
