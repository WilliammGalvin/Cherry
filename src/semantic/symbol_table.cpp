#include "cherry/semantic/symbol_table.hpp"

using namespace cherry::semantic;

SymbolTable::SymbolTable() {
    enter_scope();
}

void SymbolTable::enter_scope() {
    scopes.emplace_back();
}

void SymbolTable::exit_scope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool SymbolTable::declare(const std::string& name, SymbolType type) {
    if (is_declared(name)) {
        return false;
    }

    scopes.back()[name] = type;
    return true;
}

bool SymbolTable::is_declared(const std::string& name) const {
    for (const auto& scope : scopes) {
        if (scope.contains(name)) {
            return true;
        }
    }

    return false;
}

const SymbolType* SymbolTable::get_type(const std::string& name) const {
    for (const auto& scope : scopes) {
        if (scope.contains(name)) {
            return &scope.at(name);
        }
    }

    return nullptr;
}