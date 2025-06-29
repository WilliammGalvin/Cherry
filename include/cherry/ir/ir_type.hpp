#ifndef IR_TYPE_HPP
#define IR_TYPE_HPP
#include <stdexcept>
#include <string>

#include "cherry/ast/types.hpp"

namespace cherry::ir {

    enum IRType {
        INT, FLOAT, STRING, BOOL, VOID
    };

    inline IRType ast_type_to_ir_type(const ast::Type type) {
        switch (type) {
            case ast::Type::INT: return INT;
            case ast::Type::FLOAT: return FLOAT;
            case ast::Type::STRING: return STRING;
            case ast::Type::BOOL: return BOOL;
            case ast::Type::VOID: return VOID;

            default:
                throw std::runtime_error("Invalid IR type");
        }
    }

    inline std::string ir_type_to_str(const IRType type) {
        switch (type) {
            case INT: return "INT";
            case FLOAT: return "FLOAT";
            case STRING: return "STRING";
            case BOOL: return "BOOL";
            case VOID: return "VOID";

            default:
                throw std::runtime_error("Missing string representation for IRType enum.");
        }
    }

}

#endif //IR_TYPE_HPP
