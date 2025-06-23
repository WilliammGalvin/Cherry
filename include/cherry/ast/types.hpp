#ifndef VAR_TYPES_HPP
#define VAR_TYPES_HPP

namespace cherry::ast {

    enum Type {
        INT,
        FLOAT,
        STRING,
        BOOL,
        VOID,

        NONE
    };

    inline std::string var_type_to_str(const Type type) {
        switch (type) {
            case INT: return "INT";
            case FLOAT: return "FLOAT";
            case STRING: return "STRING";
            case BOOL: return "BOOL";
            case VOID: return "VOID";
            case NONE: return "NONE";

            default:
                throw std::runtime_error("Missing string representation of VarType.");
        }
    }

}

#endif //VAR_TYPES_HPP
