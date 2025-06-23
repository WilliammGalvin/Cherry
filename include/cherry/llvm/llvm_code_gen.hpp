#ifndef LLVM_CODE_GEN_HPP
#define LLVM_CODE_GEN_HPP

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Verifier.h>

#include "cherry/ir/ir_function_decl.hpp"
#include "cherry/ir/ir_program.hpp"
#include "cherry/ir/ir_values.hpp"
#include "cherry/printer/ast_printer.hpp"

namespace cherry::llvm {

    class LLVMCodeGen {
        ::llvm::LLVMContext context;
        std::unique_ptr<::llvm::Module> module;
        std::unique_ptr<::llvm::IRBuilder<>> builder;

        std::unordered_map<std::string, ::llvm::Value*> named_values;

        ::llvm::Type* to_llvm_type(const ir::IRNode* node);
        ::llvm::Type* to_llvm_type(const ast::Type& type);

        ::llvm::Function* lower_function(const ir::IRFunctionDecl* func);
        ::llvm::Value* lower_constant(const ir::IRConstant* constant);
        void lower_instruction(const ir::IRNode* node);

    public:
        explicit LLVMCodeGen(const std::string& module_name);

        void lower_program(const ir::IRProgram* program);

        const ::llvm::Module& get_module();
    };

}

#endif //LLVM_CODE_GEN_HPP
