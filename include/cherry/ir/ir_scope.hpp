#ifndef IR_SCOPES_HPP
#define IR_SCOPES_HPP
#include <memory>
#include <vector>

#include "ir_function.hpp"

namespace cherry::ir {

    enum ScopeVisibility {
        PUBLIC, PRIVATE, LOCAL
    };

    struct IRAbstractScope {
        ScopeVisibility visibility;
        explicit IRAbstractScope(const ScopeVisibility visibility) : visibility(visibility) {}
        virtual ~IRAbstractScope() = default;
    };

    template <typename T>
    struct IRScope : IRAbstractScope {
        std::vector<std::unique_ptr<T>> contents;

        IRScope(const ScopeVisibility visibility, std::vector<std::unique_ptr<T>>&& contents)
            : IRAbstractScope(visibility), contents(std::move(contents)) {}
    };

    struct IRGlobalScope final : IRScope<IRFunction> {
        IRGlobalScope(const ScopeVisibility visibility, std::vector<std::unique_ptr<IRFunction>>&& functions)
            : IRScope(visibility, std::move(functions)) {}
    };

    struct IRLocalScope final : IRScope<IRInstruction>, IRInstruction {
        IRLocalScope(const ScopeVisibility visibility, std::vector<std::unique_ptr<IRInstruction>>&& body)
            : IRScope(visibility, std::move(body)) {}
    };

}

#endif //IR_SCOPES_HPP
