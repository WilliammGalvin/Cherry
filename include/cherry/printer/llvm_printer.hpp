#ifndef LLVM_PRINTER_HPP
#define LLVM_PRINTER_HPP

#include <sstream>

#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>

namespace cherry::llvm_printer {

    inline void print_value(std::ostream& os, ::llvm::Value* val) {
        std::string str;
        ::llvm::raw_string_ostream rso(str);
        val->print(rso);
        os << rso.str();
    }

    inline void print_function(std::ostream& os, ::llvm::Function* func) {
        std::string str;
        ::llvm::raw_string_ostream rso(str);
        func->print(rso);
        os << rso.str();
    }

    inline void print_module(std::ostream& os, const ::llvm::Module& module) {
        std::string str;
        ::llvm::raw_string_ostream rso(str);
        module.print(rso, nullptr);
        os << rso.str();
    }

}

#endif // LLVM_PRINTER_HPP
